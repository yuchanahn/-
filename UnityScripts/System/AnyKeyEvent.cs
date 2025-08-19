using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Events;

public class AnyKeyEvent : MonoBehaviour
{
    public UnityEvent OnAnyKey;

    void Update()
    {
        if (Input.anyKeyDown)
        {
            OnAnyKey?.Invoke();
        }
    }
}
