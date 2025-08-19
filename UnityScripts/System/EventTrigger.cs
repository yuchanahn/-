using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Events;
using UnityEngine.Serialization;

public class EventTrigger : MonoBehaviour
{
    [FormerlySerializedAs("OnTrigger")] [SerializeField]
    private UnityEvent onTrigger;
    [FormerlySerializedAs("OnExit")] [SerializeField]
    private UnityEvent onExit;
    
    private void OnTriggerEnter2D(Collider2D other) {
        if(other.CompareTag("Player")) onTrigger?.Invoke();
    }
    
    private void OnTriggerExit2D(Collider2D other) {
        if(other.CompareTag("Player")) onExit?.Invoke();
    }
}
