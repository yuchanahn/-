using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class FadeIn : MonoBehaviour
{
    private SpriteRenderer spriteRenderer;
    private Color color;
    private float alpha;
    private float fadeSpeed = 1f;

    private void Start()
    {
        spriteRenderer = GetComponent<SpriteRenderer>();
        color = spriteRenderer.color;
    }

    public void StartFade(float speed)
    {
        StartCoroutine(Fade(speed));
    }

    public IEnumerator Fade(float time)
    {
        Color color = spriteRenderer.color;
        while (color.a > alpha)
        {
            color.a -= Time.deltaTime / time;
            spriteRenderer.color = color;
            yield return null;

        }
        Time.timeScale = 1;
        yield return null;
    }
}
