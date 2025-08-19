using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PCAniPlayer : MonoBehaviour
{
    [SerializeField] Animator SlashAmt;
    private void Awake()
    {
        SlashAmt = GetComponent<Animator>();
    }

    public void PlayAni(string name)
    {
        SlashAmt.gameObject.SetActive(true);
        SlashAmt.CrossFade(name, 0f);
    }

    public void EndAni(string name)
    {
        SlashAmt.gameObject.SetActive(true);
        SlashAmt.CrossFade(name, 0f);
    }
}
