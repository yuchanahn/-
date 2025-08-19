using System;
using System.Collections;
using System.Collections.Generic;
using Cinemachine;
using UnityEngine;

public class CamMinMax : MonoBehaviour
{
    private CinemachineVirtualCamera vcam;
    private PC pc;

    [SerializeField] private Transform min;
    [SerializeField] private Transform max;
    private void Start()
    {
        vcam = GetComponent<CinemachineVirtualCamera>();
        pc = FindObjectOfType<PC>();
    }

    private void Update()
    {
        vcam.Follow = pc.transform.position.x < min.position.x || pc.transform.position.x > max.position.x ? null : pc.transform;
        vcam.transform.position = pc.transform.position.x < min.position.x ? min.position : max.position;
    }
}
