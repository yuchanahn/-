using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Events;

public class RotableObject : MonoBehaviour
{
    [SerializeField] AnimationCurve speed;
    [SerializeField] AnimationCurve back_speed;

    Vector3 start_pos;

    [SerializeField] MovingPlatform EndEvent;

    [SerializeField] UnityEvent OnEnd;

    private void Awake() {
        start_pos = transform.position;
    }

    public void OnStart() {
        StartCoroutine(Movement());
    }
    public void ResetAct() {
        StartCoroutine(ResetMove());
    }

    private IEnumerator Movement() {
        float time = 0;
        while (time < 1) {
            time += Time.deltaTime;
            transform.rotation = Quaternion.Euler(0,0, speed.Evaluate(time));
            yield return null;
        }
        EndEvent.isArrival = true;
        OnEnd?.Invoke();
    }

    private IEnumerator ResetMove() {
        float time = 0;
        while (time < 1) {
            time += Time.deltaTime;
            transform.rotation = Quaternion.Euler(0, 0, 90 - back_speed.Evaluate(time));
            yield return null;
        }
    }

}
