using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Events;

public class MovingPlatform : MonoBehaviour
{
    [Tooltip("µµÂø Æ÷ÀÎÆ®")]
    public Transform desPos;
    public float speed;
    [HideInInspector] public bool isArrival;

    [SerializeField] private float backToPosTime;
    private Vector2 originPos;
    private BoxCollider2D interactionTrigger;

    [SerializeField] private UnityEvent EndedEvent;
    private void Start()
    {
        interactionTrigger = GetComponents<BoxCollider2D>()[1];
        originPos = transform.position;
        interactionTrigger.enabled = false;
    }

    private void FixedUpdate()
    {
        if (isArrival)
        {
            interactionTrigger.enabled = false;
            transform.position = Vector2.MoveTowards(transform.position, desPos.position, Time.deltaTime * speed);
            if(transform.position.y == desPos.position.y)
                StartCoroutine(BackToPos());
        }
        else
        {
            transform.position = Vector2.MoveTowards(transform.position, originPos, Time.deltaTime * speed);
        }
    }

    private void Update()
    {
        if(transform.position.y == originPos.y)
            interactionTrigger.enabled = false;
    }

    IEnumerator BackToPos()
    {
        Debug.Log("Back to Position");
        yield return new WaitForSeconds(backToPosTime);
        interactionTrigger.enabled = true;
        isArrival = false;
        EndedEvent?.Invoke();
        yield return null;
    }

    private void OnTriggerEnter2D(Collider2D collision)
    {
        if(collision.CompareTag("Player"))
        {
            collision.transform.SetParent(transform);
        }
    }

    private void OnTriggerExit2D(Collider2D collision)
    {
        if (collision.CompareTag("Player"))
            collision.transform.SetParent(null);
    }
}
