using UnityEngine;

[System.Serializable]
public class DJTimer {
    public float time;
    private float timer = 0f;
    private bool active = true;

    public bool IsEnded => this.timer >= this.time;
    public float Elapsed => this.timer;

    public DJTimer(float time, bool active = true) {
        this.time = time;
        this.timer = 0;
        this.active = active;
    }
    public DJTimer Clone() {
        return (DJTimer)this.MemberwiseClone();
    }

    public void SetActive(bool active) {
        this.active = active;
    }
    public void Reset() {
        this.timer = 0f;
    }
    public void End() {
        this.timer = this.time;
    }
    public void Tick() {
        if (this.active && this.time > this.timer) {
            this.timer += Time.deltaTime;
        }
    }
}
