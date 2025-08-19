using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Rendering.LWRP;

public class Hammer : MonoBehaviour
{
    private float hp;
    [HideInInspector] public bool isDead;

    //private SpriteRenderer spriteRenderer;
    private Animator anim;
    private AudioSource audioSource;

    [Space]
    [Header("»ç¿îµå")]
    [SerializeField] private AudioClip deadSound;

    //[SerializeField] private Sprite damagedSprite;
    [SerializeField] private ParticleSystem damagedEffect;

    private void Start()
    {
        anim = GetComponent<Animator>();
        audioSource = GetComponent<AudioSource>();
        //spriteRenderer = GetComponent<SpriteRenderer>();
    }

    void Update()
    {
        hp = GetComponent<Health>().value;

        if (hp <= 0)
        {
            //if (damagedSprite != null)
            //{
            //    anim.enabled = false;
            //    spriteRenderer.sprite = damagedSprite;
            //}

            if (!isDead)
            {
                //GetComponentInParent<Boss>().deadCount++;
                anim.SetBool("IsDamaged", true);
                audioSource.PlayOneShot(deadSound);
                if (damagedEffect != null)
                {
                    damagedEffect.gameObject.SetActive(true);
                    damagedEffect.Play();
                }

                

                isDead = true;
            }
        }
    }

}