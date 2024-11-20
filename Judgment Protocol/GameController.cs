using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Linq;
using System.IO;
using UnityEngine.UI;
using TMPro;
using static UnityEditor.Experimental.AssetDatabaseExperimental.AssetDatabaseCounters;
using UnityEngine.UIElements;
using Unity.VisualScripting;
using UnityEngine.SceneManagement;

/// <summary>
/// Manages the core gameplay logic for Judgment Protocol, character movement, 
/// handling interactions, decisions, and endings.
/// </summary>
public class GameController : MonoBehaviour
{
    // Game Data
    public List<List<string>> questions; // List of questions and their possible answers
    public List<bool> humans = new List<bool>(); // Tracks whether each character is human or AI

    // Character Movement Settings
    public List<GameObject> characters; // List of character GameObjects
    public float characterSpeed = 5;    // Speed at which characters move
    public float frequency = 1;         // Frequency of sine wave movement
    public float amplitude = 0.25f;     // Amplitude of sine wave movement
    public float fallSpeed = 1;         // Speed at which characters fall when denied

    // UI Elements
    public TMP_Text optionA, optionB;       // Option text for player choices
    public TMP_Text responseBubble, responseText; // NPC response bubble and text
    public TMP_Text questionBubble, questionText; // Question bubble and text
    public UnityEngine.UI.Button buttonA, buttonB, admit, deny; // Buttons for player choices
    public Sprite admitOn, denyOn;         // Sprites for active admit/deny buttons
    public Sprite admitOff, denyOff;       // Sprites for inactive admit/deny buttons
    public float playerTypingSpeed = 0.01f; // Typing speed for player text
    public float npcTypingSpeed = 0.05f;    // Typing speed for NPC text

    // Gameplay Settings
    public int numOfRefugees;    // Total number of refugees
    public int minHumans;        // Minimum number of humans
    public int maxHumans;        // Maximum number of humans

    // Internal Game State
    private int numOfHumans;          // Number of humans among refugees
    private bool isHuman;             // Whether the current character is human
    private List<string> question_1;  // The first selected question
    private List<string> question_2;  // The second selected question
    private bool optionSelected = false; // Whether the player has selected an option
    private bool decisionMade = false;   // Whether the player has made a decision
    private int selectedOption = 0;      // The option selected by the player
    private bool decision = false;       // The player's admit/deny decision
    private int robotsAdmitted = 0;      // Number of robots admitted
    private int humansDenied = 0;        // Number of humans denied
    private bool gameOver = false;       // Whether the game is over

    // Audio
    private MusicController musicController; // Reference to the MusicController
    private AudioSource source;              // Audio source for sound effects
    [SerializeField]
    private List<AudioClip> choiceList;      // List of audio clips for choices

    /// <summary>
    /// Initializes the game and sets up the gameplay loop.
    /// </summary>
    void Start()
    {
        /**
         * Handle the switch from menu music to gameplay music
         * as well as getting the audio source reference
         */
        if(GameObject.FindWithTag("Music"))
        {
            musicController = GameObject.FindWithTag("Music").GetComponent<MusicController>();
            musicController.SwitchToGameplayMusic();
        } else
        {
            Debug.Log("Error. No Music Found.");
        }

        source = GetComponent<AudioSource>();

        numOfHumans = Random.Range(minHumans, maxHumans + 1);

        /* Read and load Questions.txt from resources folder.
         * The question is index 0, AI answer is index 1, and human answers are indices 2 and 3.
         * so for one group: [question, aiAnswer, humanAnswer, humanAnswer]
         */
        questions = new List<List<string>>();
        /* load file at Assets/Resources/Questions.txt */
        TextAsset textAsset = Resources.Load<TextAsset>("Questions");
        if(textAsset == null)
        {
            Debug.LogError("File not Found!");
            return; // exit
        }
        questions = ParseFileContent(textAsset.text);

        // Setup bool array
        for (int i = 0; i < numOfHumans; i++)
            humans.Add(true);
        for (int i = 0; i < numOfRefugees-numOfHumans; i++)
            humans.Add(false);

        humans = ShuffleListWithOrderBy(humans);

        // Add listeners to buttons
        buttonA.onClick.AddListener(() => OnOptionSelected(1));  // 1 for option A
        buttonB.onClick.AddListener(() => OnOptionSelected(2));  // 2 for option B

        StartCoroutine(GameplayLoop());
    }

    /// <summary>
    /// Main gameplay loop. Handles phases until all refugees are processed.
    /// </summary>
    /// <returns>IEnumerator for coroutine execution.</returns>
    private IEnumerator GameplayLoop()
    {
        // Continue looping through phases until the humans list is empty
        while (humans.Count > 0)
        {    
            optionSelected = false;
            decisionMade = false;
            selectedOption = 0;
            decision = false;

            // Call the Phase coroutine and wait for it to finish
            yield return StartCoroutine(Phase());
        }

        // After the loop, perform any actions when the game is over
        gameOver = true;

        if(robotsAdmitted > 0)
        {
            SceneManager.LoadScene("Lose");
            Debug.Log("The AI infiltrated your city and took over!");
        }
        else if (humansDenied > 0)
        {
            SceneManager.LoadScene("WinAmbiguous");
            Debug.Log("The AI failed to infiltrated your city, but at least one innocent human lost their life at your hands!");
        }
        else
        {
            SceneManager.LoadScene("Win");
            Debug.Log("The AI failed to infiltrated your city, and no innocent humans lost their lives at your hands!");
        }
    }

    /// <summary>
    /// Manages the phases of the game, including character movement, questions, and decisions.
    /// </summary>
    /// <returns>IEnumerator for coroutine execution.</returns>
    private IEnumerator Phase()
    {
        // TODO: Animation of the character walking up
        int characterIndex = Random.Range(0, characters.Count);
        float sinAdder = -Time.time * frequency;
        while (characters[characterIndex].transform.position.x < 0)
        {
            if(Mathf.Sin(Time.time * frequency + sinAdder) < 0)
            {
                sinAdder += Mathf.PI;
            }
            characters[characterIndex].transform.position = new Vector3(characters[characterIndex].transform.position.x + Time.deltaTime * characterSpeed, Mathf.Sin(Time.time * frequency + sinAdder) * amplitude + 0.5f, 0);
            yield return null;
        }

        // Ask questions
        yield return StartCoroutine(AskQuestion());

        // Remove Options Text
        optionA.SetText("");
        optionB.SetText("");

        // Allow a decision
        admit.onClick.AddListener(() => OnDecision(true)); // True for the Admit Button
        deny.onClick.AddListener(() => OnDecision(false)); // False for the Deny Button
        admit.image.sprite = admitOn; // Change the sprite to the ON state
        deny.image.sprite = denyOn; // Change the sprite to the ON state
        admit.GetComponent<ButtonEvents>().on = true; // Update the Button Events script so that the hover graphics appear
        deny.GetComponent<ButtonEvents>().on = true; // Update the Button Events script so that the hover graphics appear

        // Wait until the player selects an option (either admit or deny)
        yield return new WaitUntil(() => decisionMade);

        // Disable decisions
        admit.onClick.RemoveAllListeners(); // True for the Admit Button
        deny.onClick.RemoveAllListeners(); // False for the Deny Button
        admit.image.sprite = admitOff; // Change the sprite to the OFF state
        deny.image.sprite = denyOff; // Change the sprite to the OFF state
        admit.GetComponent<ButtonEvents>().on = false; // Update the Button Events script so that the hover graphics dont appear
        deny.GetComponent<ButtonEvents>().on = false; // Update the Button Events script so that the hover graphics dont appear

        // Adjust game state based on decision outcome
        if (humans[0])
        {
            if(!decision)
            {
                humansDenied++;
                Debug.Log("You killed a human.");
            }
        } else
        {
            if(decision)
            {
                robotsAdmitted++;
                Debug.Log("You let in a robot.");
            }
        }
        
        // Remove speech bubbles
        responseBubble.gameObject.SetActive(false);
        questionBubble.gameObject.SetActive(false);

        // Remove the last interview from the list
        humans.RemoveAt(0);

        // Use decision == false for denied, decision == true for admitted
        if(decision == false)
        {
            if(characters[characterIndex].GetComponent<Scream>() != null)
                characters[characterIndex].GetComponent<Scream>().ScreamSound();
            float startTime = Time.time;
            while(characters[characterIndex].transform.position.y > -15)
            {
                characters[characterIndex].transform.position -= new Vector3(0, fallSpeed * (Time.time - startTime) * (Time.time - startTime), 0);
                yield return null;
            }
        }
        else
        {
            sinAdder = -Time.time * frequency;
            while (characters[characterIndex].transform.position.x < 15)
            {
                if (Mathf.Sin(Time.time * frequency + sinAdder) < 0)
                {
                    sinAdder += Mathf.PI;
                }
                characters[characterIndex].transform.position = new Vector3(characters[characterIndex].transform.position.x + Time.deltaTime * characterSpeed, Mathf.Sin(Time.time * frequency + sinAdder) * amplitude + 0.5f, 0);
                yield return null;
            }
        }

        characters.RemoveAt(characterIndex);
        Debug.Log("End of Phase.");
    }

    /// <summary>
    /// Handles asking questions to the current character and processing their responses.
    /// </summary>
    /// <returns>IEnumerator for coroutine execution.</returns>
    private IEnumerator AskQuestion()
    {
        // Ask 3 questions to NPC
        int playerQuestionsToAsk = 3;
        while (playerQuestionsToAsk > 0)
        {
            isHuman = humans[0];

            // Select 2 hypothetical questions from the questions list
            int index = Random.Range(0, questions.Count);
            question_1 = questions[index];
            questions.RemoveAt(index);

            index = Random.Range(0, questions.Count);
            question_2 = questions[index];
            questions.RemoveAt(index);

            // Display questions in main box
            optionA.SetText(question_1[0]);
            optionB.SetText(question_2[0]);

            // Reset selection flags
            optionSelected = false;
            selectedOption = 0;

            // Wait until the player selects an option (either button A or B)
            yield return new WaitUntil(() => optionSelected);

            // Respond to the player's selection
            if (isHuman)
            {
                int rand = (int)Random.Range(2,4);
                // If it's a human, respond with a human answer
                if (selectedOption == 1)
                {
                    string question = question_1[0];
                    string response = question_1[rand];
                    yield return StartCoroutine(DisplayMessages(question, response));
                    Debug.Log("Human Answer: " + response);
                }
                else
                {
                    string question = question_2[0];
                    string response = question_2[rand];
                    yield return StartCoroutine(DisplayMessages(question, response));
                    Debug.Log("Human Answer: " + response);
                }
            }
            else // If it's AI, respond with the AI answer
            {
                if (selectedOption == 1)
                {
                    string question = question_1[0];
                    string response = question_1[1];
                    yield return StartCoroutine(DisplayMessages(question, response));
                    Debug.Log("AI Answer: " + response);
                }
                else
                {
                    string question = question_2[0];
                    string response = question_2[1];
                    yield return StartCoroutine(DisplayMessages(question, response));
                    Debug.Log("AI Answer: " + response);
                }
            }

            playerQuestionsToAsk--;
        }
    }

    /// <summary>
    /// Displays a question and response with typing animations.
    /// </summary>
    /// <param name="question">The question text to display.</param>
    /// <param name="response">The response text to display.</param>
    /// <returns>IEnumerator for coroutine execution.</returns>
    private IEnumerator DisplayMessages(string question, string response)
    {
        // Clear the question options
        optionA.SetText("");
        optionB.SetText("");

        // Hide the npc's dialogue bubble
        responseBubble.gameObject.SetActive(false);

        // Start typing the first message in responseBubble
        questionBubble.gameObject.SetActive(true);
        yield return StartCoroutine(TypeText(questionText, questionBubble, question, playerTypingSpeed));
        
        // After the first message finishes, start typing the second message in textBubble
        responseBubble.gameObject.SetActive(true);
        yield return StartCoroutine(TypeText(responseText, responseBubble, response, npcTypingSpeed));
    }

    /// <summary>
    /// Types text letter by letter into the specified text components.
    /// </summary>
    /// <param name="textComponent">The main text component to display the message.</param>
    /// <param name="bubbleTextComponent">The speech bubble text component to display the message.</param>
    /// <param name="message">The text to type out.</param>
    /// <param name="speed">The typing speed (time between each letter).</param>
    /// <returns>IEnumerator for coroutine execution.</returns>
    private IEnumerator TypeText(TMP_Text textComponent, TMP_Text bubbleTextComponent, string message, float speed)
    {
        // Clear previous text
        textComponent.text = "";
        bubbleTextComponent.text = "";

        // Display each letter one at a time
        foreach (char letter in message)
        {
            textComponent.text += letter; // Add one letter at a time
            bubbleTextComponent.text += letter; // Add one letter at a time
            yield return new WaitForSeconds(speed); // Wait before next letter
        }
    }

    /// <summary>
    /// Called when a player selects an option (e.g., Option A or B).
    /// </summary>
    /// <param name="option">The selected option (1 for A, 2 for B).</param>
    private void OnOptionSelected(int option)
    {
        // Play an audio clip
        if (choiceList.Count > 0 && optionSelected == false)
        {
            int rand = Random.Range(0, choiceList.Count);
            source.PlayOneShot(choiceList[rand]);
        }
        // Player selected an option
        optionSelected = true;
        selectedOption = option;  // Store the selected option
        
    }

    /// <summary>
    /// Handles the player's decision to admit or deny a character.
    /// </summary>
    /// <param name="admit">True if admitting, false if denying.</param>
    private void OnDecision(bool admit)
    {
        decision = admit;
        decisionMade = true;
    }
    /// <summary>
    /// Parses the content of a file into a structured list of questions and answers.
    /// </summary>
    /// <param name="fileContent">The file content as a string.</param>
    /// <returns>A list of lists, where each inner list contains a question and answers.</returns>
    private List<List<string>> ParseFileContent(string fileContent)
    {
        List<List<string>> parsedData = new List<List<string>>();
        List<string> currentGroup = new List<string>();
        string[] lines = fileContent.Split('\n'); // split file content into lines

        foreach (string line in lines)
        {
            // Trim whitespace and check if the line is non-empty
            if (!string.IsNullOrEmpty(line))
                currentGroup.Add(line);

            // Once we have 4 elements in the current group, add it to the list and start a new group
            if (currentGroup.Count == 4)
            {
                /* Create a new list and pass that reference to parsedData */
                parsedData.Add(new List<string>(currentGroup)); // add a copy of the current group
                currentGroup.Clear(); // remove all elements, reuse same list for next group
            }
        }

        return parsedData;
    }

    /// <summary>
    /// Displays the parsed questions and answers in the debug log for verification.
    /// </summary>
    public void DisplayParsedContent()
    {
        foreach (List<string> group in questions)
        {
            Debug.Log("New Group:");
            foreach (string element in group)
                Debug.Log(element);
        }
    }

    /// <summary>
    /// Shuffles a list of boolean values using a random order.
    /// </summary>
    /// <param name="list">The list to shuffle.</param>
    /// <returns>A shuffled list.</returns>
    public List<bool> ShuffleListWithOrderBy(List<bool> list)
    {
        System.Random random = new System.Random();
        return list.OrderBy(x => random.Next()).ToList();
    }
}
