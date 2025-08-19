using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class MovealbeObject : MonoBehaviour
{
    [SerializeField] Transform target;
    [SerializeField] AnimationCurve speed;
    [SerializeField] private bool elevator;

    Vector3 start_pos;
    private void Awake() {
        start_pos = transform.position;
    }

    public void OnStart() {
        StartCoroutine(Movement());
    }
    public void OnReset() {
        transform.position = start_pos;
    }
    IEnumerator Movement()
    {
        if (!elevator)
        {
            float time = 0;
            Vector3 startPos = transform.position;
            Vector3 endPos = target.position;
            while (time < 1)
            {
                time += Time.deltaTime;
                transform.position = Vector3.Lerp(startPos, endPos, speed.Evaluate(time));
                yield return null;
            }
        }
        else if (elevator)
        {
            if(GameManager.Instance.enemyCount <= 0)
            {
                float time = 0;
                Vector3 startPos = transform.position;
                Vector3 endPos = target.position;
                while (time < 1)
                {
                    time += Time.deltaTime;
                    transform.position = Vector3.Lerp(startPos, endPos, speed.Evaluate(time));
                    yield return null;
                }
            }
        }
        

    }
}
