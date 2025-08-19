using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class BarTickSound : MonoBehaviour
{
    private AudioBinder ab;
    [SerializeField] private float max;
    private float t = 0;

    
    public void play()
    {
        ab.PlaySFXOneShot("");
    } 
    
    private void Update()
    {
        if (t >= max)
        {
            t -= max;
        }
        else
        {
            t += Time.deltaTime;
        }
    }
}
