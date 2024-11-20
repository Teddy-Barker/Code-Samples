using Pathfinding;
using System.Collections;
using UnityEngine;

/// <summary>
/// Controls the behavior of the Chain Bot enemy, including animations,
/// death logic, and weapon interaction.
/// </summary>
public class ChainBotController : MonoBehaviour
{
    // Components and references
    public Animator animator;                        // Controls animations
    public EnemyMovement enemyMovement;              // Reference to the enemy's movement behavior
    public GameObject weapon;                        // Weapon object attached to the enemy
    public GameObject player;                        // Reference to the player
    public SpriteRenderer spriteRenderer;            // Sprite renderer for visual effects
    public GameObject parent;                        // Parent object for AI-related components
    public AudioSource moveSound, footstep, deathSource; // Audio sources for movement, footsteps, and death

    // Death and weapon logic
    public float deathAnimDuration;                  // Duration of death animation
    public Vector2 weaponOffsets;                    // Weapon position offsets based on direction

    // Counters for various timings
    private float deathCounter;                      // Counter for Death Animation Sequence
    private float moveSoundcounter;                  // Counter for Movement Audio
    private float footstepcounter;                   // Counter for Footstep Audio

    // Movement tracking
    private Vector3 lastPosition;                    // Tracks the enemy's last position
    private float lastCheckTime;                     // Tracks the time of the last position check
    private float speedThreshold = 0.5f;             // Speed threshold for idle detection

    // State tracking
    private int lastFrameHealth;                     // Enemy's health in the last frame
    private bool dead;                               // Tracks whether the enemy is dead

    /// <summary>
    /// Initializes components and sets default values.
    /// </summary>
    void Start()
    {
        player = GameObject.FindWithTag("Player");
        animator = GetComponent<Animator>();
        enemyMovement = GetComponent<EnemyMovement>();
        spriteRenderer = GetComponent<SpriteRenderer>();

        lastPosition = transform.position;
        lastCheckTime = Time.time;

        lastFrameHealth = enemyMovement.health;
        footstepcounter = 0.15f; // Initial delay for footstep sounds
    }

    /// <summary>
    /// Updates the enemy's behavior every frame.
    /// </summary>
    void Update()
    {
        HandleMovementSounds();
        UpdateWeaponEffects();
        animator.SetFloat("Speed", CalculateSpeed());
        CalculateLookDirection();
        HandleDeathLogic();
    }

    /// <summary>
    /// Plays movement and footstep sounds based on timing intervals.
    /// </summary>
    private void HandleMovementSounds()
    {
        // Play movement sound
        if (moveSoundcounter > 0.35f)
        {
            moveSound.pitch = Random.Range(0.85f, 1f);
            moveSound.volume = Random.Range(0f, 0.2f);
            moveSound.PlayOneShot(moveSound.clip);
            moveSoundcounter = 0;
        }
        else
        {
            moveSoundcounter += Time.deltaTime;
        }

        // Play footstep sound
        if (footstepcounter > 0.35f)
        {
            footstep.pitch = Random.Range(0.85f, 1f);
            footstep.volume = Random.Range(0.85f, 1f);
            footstep.PlayOneShot(footstep.clip);
            footstepcounter = 0;
        }
        else
        {
            footstepcounter += Time.deltaTime;
        }
    }

    /// <summary>
    /// Updates the weapon's visual effects to match the enemy's state.
    /// </summary>
    private void UpdateWeaponEffects()
    {
        weapon.GetComponent<SpriteRenderer>().material.SetFloat("_FlashAmount", enemyMovement.spriteRenderer.material.GetFloat("_FlashAmount"));
        weapon.GetComponent<SpriteRenderer>().material.SetColor("_FlashColor", enemyMovement.spriteRenderer.material.GetColor("_FlashColor"));
    }

    /// <summary>
    /// Handles death logic, including animations, disabling components, and object destruction.
    /// </summary>
    private void HandleDeathLogic()
    {
        if (enemyMovement.health <= 0)
        {
            dead = true;

            // Disable movement and collisions
            GetComponent<AIPath>().canMove = false;
            GetComponent<PolygonCollider2D>().enabled = false;
            enemyMovement.dead = true;

            // Play death animation
            animator.SetBool("Death", true);
            weapon.SetActive(false);

            // Destroy the object after the animation ends
            if (deathCounter < deathAnimDuration)
            {
                deathCounter += Time.deltaTime;
            }
            else
            {
                Destroy(gameObject);
            }
        }
    }

    /// <summary>
    /// Calculates the enemy's facing direction based on the player's position.
    /// </summary>
    private void CalculateLookDirection()
    {
        if (player.transform.position.x < transform.position.x)
        {
            spriteRenderer.flipX = true;
            weapon.transform.localScale = new Vector3(-1, 1, 1);
            weapon.transform.position = new Vector2(transform.position.x - weaponOffsets.x, transform.position.y + weaponOffsets.y);
        }
        else
        {
            spriteRenderer.flipX = false;
            weapon.transform.localScale = new Vector3(1, 1, 1);
            weapon.transform.position = new Vector2(transform.position.x + weaponOffsets.x, transform.position.y + weaponOffsets.y);
        }
    }

    /// <summary>
    /// Calculates the speed of the enemy based on distance traveled since the last check.
    /// </summary>
    /// <returns>The calculated speed.</returns>
    private float CalculateSpeed()
    {
        // Calculate time elapsed since last check
        float deltaTime = Time.time - lastCheckTime;

        // Calculate distance traveled
        float distance = Vector3.Distance(lastPosition, transform.position);

        // Update last position and check time
        lastPosition = transform.position;
        lastCheckTime = Time.time;

        return distance / deltaTime;
    }
}
