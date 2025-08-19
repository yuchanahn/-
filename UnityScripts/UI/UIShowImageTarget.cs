using System;
using System.Collections;
using System.Collections.Generic;
using TMPro;
using UnityEngine;
using UnityEngine.Serialization;
using UnityEngine.UI;

public class UIShowImageTarget : MonoBehaviour
{
    [SerializeField] Image image;
    [SerializeField] Transform imageTarget;
    [SerializeField] TextMeshProUGUI text;
    [SerializeField] private bool followTarget;
    
    private IEnumerator cur;
    

    private Transform real_target = null;
    private  Camera cam;

    private void Awake()
    {
        cam = FindObjectOfType<Camera>();
    }

    private void Update()
    {
        if (!followTarget) return;
        if (!real_target) return;
        
        image.GetComponent<RectTransform>().position = cam.WorldToScreenPoint(real_target.position) + Vector3.up * 200;
    }

    public void SetImageTarget(Transform target)
    {
        imageTarget = target;
        followTarget = true;
    }
    public void Show(string msg)
    {
        if(cur is not null) StopCoroutine(cur);
        text.text = msg;
        real_target = imageTarget;
        cur = FadeIn(() =>
        {
        });
        StartCoroutine(cur);
    }

    public void Hide()
    {
        if(cur is not null) StopCoroutine(cur);
        cur = FadeOut(() =>
        {
            real_target = null;
        });
        StartCoroutine(cur);
    }
    
    public void HideImmediate()
    {
        if(cur is not null) StopCoroutine(cur);
        image.color = new Color(1,1,1,0);
        text.color = new Color(1,1,1,0);
    }

    private IEnumerator FadeIn(Action end){
        Color color = image.color;
        var textColor = text.color;
        while(color.a < 1){
            color.a += 0.01f;
            textColor.a += 0.01f;
            image.color = color;
            text.color = textColor;
            yield return new WaitForSeconds(0.01f);
        }
        end?.Invoke();
    }
    private IEnumerator FadeOut(Action end){
        Color color = image.color;
        var textColor = text.color;
        while(color.a > 0){
            color.a -= 0.01f;
            textColor.a -= 0.01f;
            image.color = color;
            text.color = textColor;
            yield return new WaitForSeconds(0.01f);
        }
        end?.Invoke();
    }
}