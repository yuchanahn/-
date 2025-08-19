using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Events;

enum eInteractionType {
    Key,
    Trigger,
}


public class InteractionObject : MonoBehaviour
{
    [SerializeField] eInteractionType type;
    [SerializeField] private new string tag = "Player";
    [SerializeField] UnityEvent OnTiggerEnter;
    [SerializeField] UnityEvent OnkeyEvent;
    [SerializeField] YCInputSystem Input;

    private void Awake() {
        Input = FindObjectOfType<YCInputSystem>();
    }

    private void OnTriggerEnter2D(Collider2D collision)
    {
        if (!collision.CompareTag(tag)) return;
        switch (type)
        {
            case eInteractionType.Trigger:
                OnTiggerEnter?.Invoke();
                break;
            case eInteractionType.Key:
                Input.key.get_event_of_name("Interaction").press_ev = _ => OnkeyEvent?.Invoke();
                break;
            default:
                throw new ArgumentOutOfRangeException();
        }
    }
    private void OnTriggerExit2D(Collider2D collision)
    {
        if (!collision.CompareTag(tag)) return;
        if (type == eInteractionType.Key) Input.key.get_event_of_name("Interaction").press_ev = null;
    }
}
