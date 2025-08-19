using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class UIAnimSequence : MonoBehaviour
{
    public void PlayAnim()
    {
        Animator anim = GetComponent<Animator>();
        anim.Play("AnimName");
    }
}