using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class StoryItem : MonoBehaviour
{
    public Image fadeImage;
    public GameObject storyImage;

    [SerializeField] private float fadeInCount;
    [SerializeField] private float fadeOutCount;

    public bool isPause;

    private YCInputSystem player;

    private void Awake()
    {
        isPause = false;

    }

    private void Update()
    {
        if (isPause == true)
        {
            Time.timeScale = 0;
            // ?????????? ????? ??¨צ? ??????? ????
            if(player.key.get_event_of_name("Attack").state || player.key.get_event_of_name("UIOpenSetting").state)
            {
                isPause = false;
            }

            return;
        }
        else if (isPause == false)
        {
            StartCoroutine(FadeIn(fadeInCount));
            //Time.timeScale = 1;
            return;
        }

    }

    private void OnTriggerEnter2D(Collider2D collision)
    {
        Debug.Log("?תפ");
        if(collision.CompareTag("Player"))
        {
            //player = collision.GetComponent<YCInputSystem>().key.get_event_of_name("Attack")
            player = collision.GetComponent<YCInputSystem>();
            isPause = true;
            GetComponent<BoxCollider2D>().enabled = false;
            StartCoroutine(FadeOut(fadeOutCount));
        }
    }

    // ???? ????? ----------------------------
    // ???????
    public IEnumerator FadeIn(float time)
    {
        Color color = fadeImage.color;
        while (color.a > 0f)
        {
            color.a -= Time.unscaledDeltaTime / time;
            fadeImage.color = color;
            yield return new WaitForSeconds(time);
            
        }
        Time.timeScale = 1;
        yield return null;
    }

    // ???????
    public IEnumerator FadeOut(float time)
    {
        Color color = fadeImage.color;
        while (color.a < 1f)
        {
            color.a += Time.unscaledDeltaTime / time;
            fadeImage.color = color;
            yield return null;
        }
    }
    // ???? ????? ----------------------------
}
