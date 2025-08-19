using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Events;

public class EventAction : MonoBehaviour
{
    public bool IsStarted
    {
        get => isStarted;
        set
        {
            isStarted = value;
        }
    }

    [SerializeField] UnityEvent act;
    [SerializeField] UnityEvent END;
    [SerializeField] private bool isStarted = false;

    public void StartEvent()
    {
        if(IsStarted) act?.Invoke();
    }
    
    public void End(){
        if(IsStarted) END?.Invoke();
        IsStarted = false;
    }
    
    public void StopEvent()
    {
        IsStarted = false;
    }
}
