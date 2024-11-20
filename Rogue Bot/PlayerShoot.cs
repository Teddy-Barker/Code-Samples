using System.Collections;
using System.Collections.Generic;
using UnityEngine;

/// <summary>
/// Handles the player's shooting mechanics, including basic projectile shooting 
/// and ammo management.
/// </summary>
public class PlayerShoot : MonoBehaviour
{
    // Transform reference for where projectiles spawn
    public Transform shootingPoint;

    // Projectile prefab
    public GameObject projectile;

    // Player stats and settings
    public float fireRate;             // Rate of fire in seconds
    public int maxAmmo;                // Maximum ammo the player can hold
    public int ammoCount;              // Current ammo count
    public int damage;                 // Base damage of the player's projectiles

    private float fireRateCounter;     // Tracks time since last shot
    private Inventory inventory;       // Reference to the player's inventory
    private GameObject inventoryUI;    // UI for inventory
    private GameObject skillTreeUI;    // UI for the skill tree
    public AudioSource shootSource;    // Audio source for shooting sound effects

    /// <summary>
    /// Initializes references to inventory and UI components.
    /// </summary>
    void Awake()
    {
        inventoryUI = GameObject.FindWithTag("InventoryUI");
        skillTreeUI = GameObject.FindWithTag("SkillTree");
        inventory = GameObject.FindWithTag("GameManager").GetComponent<Inventory>();
    }

    /// <summary>
    /// Sets up initial fire rate counter.
    /// </summary>
    void Start()
    {
        fireRateCounter = fireRate;
    }

    /// <summary>
    /// Updates the fire rate counter each frame.
    /// </summary>
    void Update()
    {
        if (fireRateCounter < fireRate)
        {
            fireRateCounter += Time.deltaTime;
        }
    }

    /// <summary>
    /// Executes the shooting logic, including ammo management and UI state checks.
    /// </summary>
    public void Shoot()
    {
        // Prevent shooting if UI elements are active
        if (inventoryUI.activeSelf || skillTreeUI.activeSelf || gameObject.GetComponent<PlayerMovement>().shopUI != null)
        {
            return;
        }

        // Ensure the fire rate cooldown is complete
        if (fireRateCounter < fireRate)
        {
            return;
        }

        // Check for available ammo in inventory
        bool ammoFound = false;
        foreach (Item item in inventory.items)
        {
            if (item.name == "Ammo")
            {
                Ammo ammoItem = (Ammo)item;
                if (ammoItem.ammoAmount > 0)
                {
                    ammoItem.ammoAmount--;
                    ammoItem.canSell = false;
                    ammoFound = true;

                    // Remove the item if ammo is depleted
                    if (ammoItem.ammoAmount <= 0)
                    {
                        inventory.Remove(item);
                    }

                    fireRateCounter = 0; // Reset fire rate cooldown
                    break;
                }
            }
        }

        // Abort if no ammo was found
        if (!ammoFound)
        {
            return;
        }

        // Play shooting sound effect with randomized pitch and volume
        shootSource.volume = Random.Range(0.5f, 0.75f);
        shootSource.pitch = Random.Range(0.85f, 1f);
        shootSource.Play();

        // Instantiate the projectile
        GameObject copy = Instantiate(projectile, shootingPoint.position, shootingPoint.rotation);
        copy.GetComponent<Projectile>().damage = damage;
    }
}
