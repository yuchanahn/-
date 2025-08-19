using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;
using UnityEngine.Events;

[Serializable]
public struct EventSqData
{
    public string name;
    public UnityEvent sequences;
}

public class EventSequence : MonoBehaviour
{
    [SerializeField] List<EventSqData> eventSequences;

    public void Call(string eventName)
    {
        var eventSequence = eventSequences.FirstOrDefault(x => x.name == eventName);
        eventSequence.sequences?.Invoke();
    }
}
