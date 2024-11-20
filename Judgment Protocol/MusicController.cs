using System.Collections;
using UnityEngine;

/// <summary>
/// Controls background music transitions between game states (e.g., menu and gameplay).
/// Implements crossfade transitions for a smooth audio experience.
/// </summary>
public class MusicController : MonoBehaviour
{
    public static MusicController musicController; // Singleton instance for global access

    [SerializeField]
    private AudioClip menuMusic;        // Background music for the menu
    [SerializeField]
    private AudioClip gameplayMusic;    // Background music for gameplay
    [SerializeField]
    private AudioSource audioSource1;   // Primary audio source
    [SerializeField]
    private AudioSource audioSource2;   // Secondary audio source
    [SerializeField]
    private float fadeDuration = 1f;    // Duration of the crossfade in seconds

    /// <summary>
    /// Ensures the singleton pattern and persists the music controller across scenes.
    /// </summary>
    void Awake()
    {
        if (musicController != null)
        {
            Destroy(gameObject); // Prevent duplicates
            return;
        }

        musicController = this;
        DontDestroyOnLoad(gameObject); // Keep this object between scene transitions
    }

    /// <summary>
    /// Starts the menu music when the game launches.
    /// </summary>
    public void Start()
    {
        audioSource1.PlayOneShot(menuMusic); // Play menu music
    }

    /// <summary>
    /// Transitions the background music to gameplay music using a crossfade.
    /// </summary>
    public void SwitchToGameplayMusic()
    {
        StartCoroutine(Crossfade(audioSource1, audioSource2, gameplayMusic));
    }

    /// <summary>
    /// Crossfades between two audio sources over a specified duration.
    /// </summary>
    /// <param name="currentSource">The currently playing audio source to fade out.</param>
    /// <param name="newSource">The new audio source to fade in.</param>
    /// <param name="newClip">The audio clip to assign to the new source.</param>
    private IEnumerator Crossfade(AudioSource currentSource, AudioSource newSource, AudioClip newClip)
    {
        float elapsedTime = 0f;

        // Prepare the new audio source
        newSource.clip = newClip;
        newSource.volume = 0f; // Start silent
        newSource.Play();

        while (elapsedTime < fadeDuration)
        {
            // Gradually fade out the current source
            currentSource.volume = Mathf.Lerp(1f, 0f, elapsedTime / fadeDuration);

            // Gradually fade in the new source
            newSource.volume = Mathf.Lerp(0f, 1f, elapsedTime / fadeDuration);

            elapsedTime += Time.deltaTime;
            yield return null; // Wait for the next frame
        }

        // Finalize the transition
        currentSource.volume = 0f;
        newSource.volume = 1f;

        // Stop the current (faded out) audio source
        currentSource.Stop();
    }
}
