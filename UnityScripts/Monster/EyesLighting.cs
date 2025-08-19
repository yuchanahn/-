using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Rendering.Universal;

public class EyesLighting : MonoBehaviour
{
    private bool isRange;
    [SerializeField] private float lightOffTime;
    public Light2D Light;

    private void Update()
    {
        if(isRange)
        {
            StartCoroutine(SetActiveFalse(lightOffTime));
        }
    }

    private void OnTriggerEnter2D(Collider2D collision)
    {
        if(collision.gameObject.CompareTag("Player"))
        {
            Light.enabled = true;
            isRange = true;
        }
    }

    private void OnTriggerExit2D(Collider2D collision)
    {
        if (collision.gameObject.CompareTag("Player"))
        {
            isRange = false;
        }
    }

    IEnumerator SetActiveFalse(float time)
    {
        yield return new WaitForSeconds(time);
        Light.enabled = false;
        yield return null;
    }
}
