using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class HiddenRoom : MonoBehaviour
{
    [SerializeField] private float fadeInCount;
    [SerializeField] private float fadeOutCount;
    [SerializeField]
    [Range(0, 1f)] private float alpha;

    private SpriteRenderer sprite;

    private void Start()
    {
        sprite = GetComponent<SpriteRenderer>();
    }

    private void OnTriggerStay2D(Collider2D collision)
    {
        if(collision.CompareTag("Player"))
            StartCoroutine(FadeIn(fadeInCount));
    }

    private void OnTriggerExit2D(Collider2D collision)
    {
        if (collision.CompareTag("Player"))
        {
            StartCoroutine(FadeOut(fadeOutCount));
        }
    }

    public IEnumerator FadeIn(float time)
    {
        Color color = sprite.color;
        while (color.a > alpha)
        {
            color.a -= Time.deltaTime / time;
            sprite.color = color;
            yield return null;

        }
        Time.timeScale = 1;
        yield return null;
    }

    // ÆäÀÌµå¾Æ¿ô
    public IEnumerator FadeOut(float time)
    {
        Color color = sprite.color;
        while (color.a < 1f)
        {
            color.a += Time.deltaTime / time;
            sprite.color = color;
            yield return null;
        }
    }
}
