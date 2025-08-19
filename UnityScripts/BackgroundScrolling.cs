using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class BackgroundScrolling : MonoBehaviour
{
    [Header("BackScrolling Speed")]
    [SerializeField] private float speed = 4f;

    private Vector3 startPos;
    private BackgroundScrolling bs;

    [Header("Sound")]
    private AudioSource AS;
    [Tooltip("ac[0] = ø§∫£º“∏Æ / ac[1] = πˆ∆∞ / ac[2] = ∏ÿ√„")]
    [SerializeField] AudioClip[] ac;
    [SerializeField] private float soundWaitSec;

    private void Start()
    {
        startPos = transform.position;
        bs = GetComponent<BackgroundScrolling>();
        AS = GetComponent<AudioSource>();
        StartCoroutine(ElevatorDown());
        //AS.PlayOneShot(ac[1]);
    }

    private void Update()
    {
        if (GameManager.Instance.enemyCount == 0)
        {
            AS.PlayOneShot(ac[2]);
            bs.enabled = false;
        }

        transform.Translate(-Vector3.down * speed * Time.deltaTime);

        if (transform.position.y > -310f)
        {
            transform.position = startPos;
        }

        
    }

    IEnumerator ElevatorDown()
    {
        yield return new WaitForSeconds(soundWaitSec);
        AS.PlayOneShot(ac[0]);
        yield return null;
    }
}
