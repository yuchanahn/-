using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Events;

namespace UI
{
    [Serializable]
    public class TCObject
    {
        public float delay;
        public UnityEvent action;
        public bool endCheck;
    }

    public class UITimeControl : MonoBehaviour
    {
        [SerializeField] bool autoStart;
        [SerializeField] List<TCObject> timeControlObjects;
        [SerializeField] CutsceneView cutsceneView;
        private void Start()
        {
            if(autoStart) Run();
        }

        public void Run()
        {
            StartCoroutine(Playing());
        }
        
        IEnumerator Playing()
        {
            TCObject beforeTcObjects = null;
            foreach (var tcObject in timeControlObjects)
            {
                if (beforeTcObjects is { endCheck: true })
                    while (cutsceneView.IsPlaying)
                        yield return new WaitForSeconds(0.1f);

                yield return new WaitForSeconds(tcObject.delay);
                tcObject.action?.Invoke();
                beforeTcObjects = tcObject;
            }
        }
        
    }
}