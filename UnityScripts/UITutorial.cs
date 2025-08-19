using System.Collections;
using System.Collections.Generic;
using TMPro;
using UnityEngine;

public class UITutorial : MonoBehaviour
{
    [SerializeField] private SpriteRenderer UIsr;
    [SerializeField] private TextMeshProUGUI UIKey;
    [SerializeField] private float fadeInCount;
    [SerializeField] private float fadeOutCount;
    
    private void OnTriggerEnter2D(Collider2D collision) {
        if (collision.gameObject.tag == "Player") {
            StartCoroutine(FadeOut(fadeInCount));
        }
    }

    private void OnTriggerExit2D(Collider2D collision) {
        if (collision.gameObject.tag == "Player") {
            StartCoroutine(FadeIn(fadeOutCount));
        }
    }

    public IEnumerator FadeIn(float time) {
        Color color = UIsr.color;
        while (color.a > 0f) {
            color.a -= Time.deltaTime / time;
            UIsr.color = color;
            UIKey.color = color;
            yield return null;

        }
        Time.timeScale = 1;
        yield return null;
    }

    // 페이드아웃
    public IEnumerator FadeOut(float time) {
        Color color = UIsr.color;
        while (color.a < 1f) {
            color.a += Time.deltaTime / time;
            UIsr.color = color;
            UIKey.color = color;
            yield return null;
        }
    }
}
