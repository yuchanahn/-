using System.Collections;
using System.Collections.Generic;
using UnityEngine;

enum GearState
{
    Idle,
    Rotating,
    Resetting
}

public class GearRotator : MonoBehaviour
{
    [SerializeField] private float rotSpeed;
    [SerializeField] private float rotTime;
    [SerializeField] private float resetTime;
    private float rotTimer;
    private GearState state;

    void Update()
    {
        switch (state)
        {
            case GearState.Idle:
                state = GearState.Resetting;
                break;
            case GearState.Rotating:
                rotTimer += Time.deltaTime;
                if (rotTimer <= rotTime)
                {
                    transform.Rotate(0, 0, rotSpeed * Time.deltaTime);
                }
                else if (rotTimer >= resetTime)
                {
                    rotTimer = 0;
                    state = GearState.Resetting;
                }

                break;
            case GearState.Resetting:
                rotTimer += Time.deltaTime;
                if(rotTimer >= resetTime)
                {
                    rotTimer = 0;
                    state = GearState.Rotating;
                }
                break;
        }
    }
    
    
}
