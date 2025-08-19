using Cinemachine;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CameraFOV : MonoBehaviour
{
    CinemachineVirtualCamera vcam;
    [SerializeField] private Vector3 labFOV = new Vector3(0, 5.8f, -5.8f);
    [SerializeField] private Vector3 originFOV = new Vector3(0, 20f, -11.97f);
    [SerializeField] private float zoomSpeed = 3f;
    private bool bLab;

    private void Start()
    {
        vcam = GameObject.Find("CM vcam1").GetComponent<CinemachineVirtualCamera>();
    }

    private void Update()
    {
        if (bLab)
        {
            var transposer = vcam.GetCinemachineComponent<CinemachineTransposer>();
            transposer.m_FollowOffset = Vector3.Lerp(transposer.m_FollowOffset, labFOV, Time.deltaTime * zoomSpeed);
        }
        else
        {
            var transposer = vcam.GetCinemachineComponent<CinemachineTransposer>();
            transposer.m_FollowOffset = Vector3.Lerp(transposer.m_FollowOffset, originFOV, Time.deltaTime * zoomSpeed);
        }
    }

    private void OnTriggerEnter2D(Collider2D collision)
    {
        if (collision.CompareTag("Player"))
        {
            bLab = true;
            //var transposer = vcam.GetCinemachineComponent<CinemachineTransposer>();
            //transposer.m_FollowOffset = labFOV;
        }
    }

    private void OnTriggerExit2D(Collider2D collision)
    {
        if (collision.CompareTag("Player"))
        {
            bLab = false;
            //var transposer = vcam.GetCinemachineComponent<CinemachineTransposer>();
            //transposer.m_FollowOffset = originFOV;
        }
    }
}
