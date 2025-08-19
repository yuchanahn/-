using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class UIRot : MonoBehaviour
{
    [SerializeField] float speed;
    void Update()
    {
        GetComponent<RectTransform>().Rotate(0, 0, speed * Time.deltaTime);
    }
}
