using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using Pathfinding;

/// <summary>
/// Handles enemy movement, AI behavior, health, and interactions with the player.
/// </summary>
public class EnemyMovement : MonoBehaviour
{
    // Health and visual feedback
    public int health;                              // Enemy health
    public SpriteRenderer spriteRenderer;           // Reference to the sprite renderer for visual effects
    public float whiteFlashTime;                    // Duration of the white flash effect when damaged
    private float whiteFlashCounter;                // Tracks remaining time for white flash effect

    // Movement and AI
    private AIPath aiPath;                          // Pathfinding AI component for movement
    public AIDestinationSetter aiDestinationSetter; // Sets the target for AI pathfinding
    public float seeRadius = 10f;                   // Radius within which the enemy can "see" the player
    public bool hasSeen;                            // Whether the enemy has spotted the player
    public bool dead;                               // Whether the enemy is dead

    // Player reference
    public GameObject player;                       // Reference to the player

    // Money drop
    public GameObject moneyItem;                    // Money item prefab
    public int moneyAmount;                         // Amount of money to drop on death

    /// <summary>
    /// Initializes enemy components and sets up AI pathfinding.
    /// </summary>
    void Start()
    {
        spriteRenderer = GetComponent<SpriteRenderer>();
        spriteRenderer.material.SetFloat("_FlashAmount", 0);

        player = GameObject.FindWithTag("Player");

        aiPath = GetComponent<AIPath>();
        aiDestinationSetter = GetComponent<AIDestinationSetter>();
        rb = GetComponent<Rigidbody2D>();

        if (player != null)
        {
            aiDestinationSetter.target = player.transform;
        }
        aiPath.canMove = false; // Disable movement initially
    }

    /// <summary>
    /// Updates enemy behavior each frame.
    /// </summary>
    void Update()
    {
        // Check if the player is within the "see" radius
        if (player != null && Vector3.Distance(transform.position, player.transform.position) <= seeRadius && !hasSeen && !dead)
        {
            if (aiPath != null)
                aiPath.canMove = true;
            hasSeen = true;
        }

        // Handle white flash effect when damaged
        if (whiteFlashCounter > 0)
        {
            spriteRenderer.material.SetFloat("_FlashAmount", 0.5f);
            whiteFlashCounter -= Time.deltaTime;
        }
        else
        {
            spriteRenderer.material.SetFloat("_FlashAmount", 0);
        }

        // If the enemy has a Rigidbody and is stationary, resume movement
            // This is specifically for the knockback effect to work as intended
        if (!dead)
        {
            if (!aiPath.canMove && rb.velocity.magnitude < 0.5f && hasSeen)
            {
                aiPath.canMove = true;
            }
        }
    }

    /// <summary>
    /// Reduces enemy health and handles death logic.
    /// </summary>
    /// <param name="damage">Amount of damage to inflict.</param>
    public void DecreaseHealth(int damage)
    {
        whiteFlashCounter = whiteFlashTime; // Trigger white flash effect
        health -= damage;

        if (health <= 0 && !dead)
        {
            dead = true; // Mark enemy as dead

            // Drop money if applicable
            if (moneyAmount > 0)
            {
                GameObject tempMoney = Instantiate(moneyItem, transform.position, Quaternion.identity);
                tempMoney.GetComponent<MoneyPickup>().amount = moneyAmount;
            }
        }
    }
}
