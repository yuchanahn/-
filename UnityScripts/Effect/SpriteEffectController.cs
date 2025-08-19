using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class SpriteEffectController : MonoBehaviour
{
    Animator animator;

    public AnimationClip ani;
    public Action OnAniStart;
    public Action OnAniEnd;

    void Start()
    {
        animator = GetComponent<Animator>();
    }

    void AniStart()
    {
        animator.CrossFade(ani.name, 0f);
        OnAniStart?.Invoke();
    }
    private void Update()
    {
        if(animator.GetCurrentAnimatorStateInfo(0).IsName(ani.name) &&
           animator.GetCurrentAnimatorStateInfo(0).normalizedTime >= 1.0f)
        {
            OnAniEnd?.Invoke();
        }
    }
}
