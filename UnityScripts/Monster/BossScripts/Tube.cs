using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System;

public class Tube : MonoBehaviour
{
    private float hp;
    private bool isDead;
    private AudioSource audioSource;
    private SpriteRenderer sr;
    public Sprite sprite;
    [SerializeField] private AudioClip brokenSound;
    
    public bool upTube;

    [DrawIf("upTube", true)]
    [SerializeField] private Health health;

    private void Start()
    {
        sr = GetComponent<SpriteRenderer>();
        audioSource = GetComponent<AudioSource>();
    }

    void Update()
    {
        if(!upTube)
        {
            hp = GetComponent<Health>().value;
        }
        else
        {
            hp = health.value;
        }

        if (hp <= 0 && !isDead)
        {
            GetComponentInParent<Boss>().deadCount++;
            sr.sprite = sprite;
            PlaySound(brokenSound);
            isDead = true;
        }
    }

    private void PlaySound(AudioClip clip, bool bLoop = false)
    {
        if (clip != null && !audioSource.isPlaying)
        {
            audioSource.clip = clip;
            audioSource.loop = bLoop;
            audioSource.Play();
            Debug.Log(audioSource.clip);
        }
    }
}
