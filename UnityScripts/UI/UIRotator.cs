using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class UIRotator : MonoBehaviour
{
    [SerializeField] private float RotateSpeed;

    [Space] [Header("인게임중 변경 X 게임시작전 변경해 주세요.")] [SerializeField]
    private bool delayRot;

    [SerializeField] private float nextRotTime;
    [SerializeField] private float delayRotSpeed;
    RectTransform rt;
    private Transform trans;
    private float rotZ;
    private bool isRot;
    //    private 
    
    private void Start()
    {
        rt = GetComponent<RectTransform>();
        trans = GetComponent<Transform>();
        rotZ = transform.rotation.z;
    }

    // Update is called once per frame
    void Update()
    {
        if (!delayRot)
        {
            if (rt != null)
                rt.Rotate(new Vector3(0, 0, RotateSpeed));
            else if (rt != trans)
                trans.Rotate(new Vector3(0, 0, RotateSpeed));
        }
        else if (isRot)
        {
            
        }
    }
}