using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class ImageAlpha : MonoBehaviour
{
    [SerializeField] private float fadeInCount;
    [SerializeField] private float fadeOutCount;
    [SerializeField]
    [Range(0,1)] private float alpha;

    private Color img;

    private void Start()
    {
        img = GetComponent<SpriteRenderer>().color;
    }

    private void OnTriggerStay2D(Collider2D collision)
    {
        if(collision.CompareTag("Player"))
            StartCoroutine(FadeIn(fadeInCount));
    }

    private void OnTriggerExit2D(Collider2D collision)
    {
        if (collision.CompareTag("Player"))
            StartCoroutine(FadeOut(fadeOutCount));
    }

    public IEnumerator FadeIn(float time)
    {
        Color color = img;
        while (color.a > alpha)
        {
            color.a -= Time.deltaTime / time;
            img.a = color.a;
            yield return null;

        }
        Time.timeScale = 1;
        yield return null;
    }

    // ÆäÀÌµå¾Æ¿ô
    public IEnumerator FadeOut(float time)
    {
        Color color = img;
        while (color.a < alpha)
        {
            color.a += Time.deltaTime / time;
            img.a = color.a;
            yield return null;
        }
    }
}
