using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Events;

public class Boss : MonoBehaviour
{
     public int deadCount = 0;
    [HideInInspector] public bool isDead;
    [SerializeField] UnityEvent onDeath;
    private void Update()
    {
        if (isDead) return;
        if (deadCount < 4) return;
        isDead = true;
        Debug.Log("º¸½º »ç¸Á");
        onDeath?.Invoke();
    }
}
