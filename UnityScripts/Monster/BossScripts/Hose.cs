using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System;

public class Hose : MonoBehaviour
{
    [SerializeField] private Health parernt;
    [SerializeField] private AudioClip cutSount;
    private Animator anim;
    private AudioSource audioSource;
    private bool isdead;
    private void Start()
    {
        anim = GetComponent<Animator>();
        audioSource = GetComponent<AudioSource>();
    }

    void Update()
    {
        if (parernt.value <= 0 && !isdead)
        {
            anim.SetBool("IsDamaged", true);
            PlaySound(cutSount);
            isdead = true;
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
