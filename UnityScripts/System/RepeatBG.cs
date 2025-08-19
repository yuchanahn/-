using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class RepeatBG : MonoBehaviour
{
    [SerializeField] [Range(1f, 20f)] private float speed = 3f;
    [SerializeField] private float posValue;

    private Vector2 startPos;
    private float newPos;

    private void Start()
    {
        startPos = transform.position;
    }

    private void Update()
    {
        newPos = Mathf.Repeat(Time.time * speed, posValue);
        transform.position = startPos + Vector2.right * newPos;
    }
}
