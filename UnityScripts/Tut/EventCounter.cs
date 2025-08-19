using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Events;

public class EventCounter : MonoBehaviour
{
    public UnityEvent endEvent;
    public int counter = 0;
    public int countMax = 0;

    public bool isCountStarted { get; set; } = false;

    public void AddCount()
    {
        if (!isCountStarted) return;
        counter++;
        if(counter >= countMax)
        {
            endEvent.Invoke();
        }
    }
}
