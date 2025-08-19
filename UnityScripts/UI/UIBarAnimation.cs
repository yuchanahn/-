using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class UIBarAnimation : MonoBehaviour {

    [SerializeField] private AnimationCurve speed;
    [SerializeField] Slider target;
    [SerializeField] Slider anim;
    [SerializeField] int dir = 1;
    float t = 1f;

    float min = -1;
    float max = -1;

    private void Awake() {
        
    }
    
    
    
    private void Update() {
        t += Time.deltaTime;
        if (Math.Abs(min - target.value) > 0.001f) {
            t = 0;
        }
        min = target.value;
        max = anim.value;

        if(dir > 0) {
            anim.value = Mathf.Max(anim.value - dir * speed.Evaluate(t) * Time.deltaTime, min);
        } else {
            anim.value = Mathf.Min(anim.value - dir * speed.Evaluate(t) * Time.deltaTime, min);
        }
        
    }
}
