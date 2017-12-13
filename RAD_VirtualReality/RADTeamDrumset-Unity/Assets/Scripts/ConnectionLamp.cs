using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ConnectionLamp : MonoBehaviour {
    private bool prevConnect;
    private Material shade;
    private Light light;

    WebConnection script;

    // Use this for initialization
    void Start () {
        this.script = GameObject.Find("Ground").GetComponent<WebConnection>();
        prevConnect = false;

        shade = this.transform.Find("LampShade").GetComponent<Renderer>().material;
        light = this.transform.Find("Light").GetComponent<Light>();
        shade.SetColor("_EmissionColor", Color.red);
        light.color = Color.red;
    }
	
	// Update is called once per frame
	void Update () {
        bool connect = script.isConnected();
        if (connect && !prevConnect)
        {
            shade.SetColor("_EmissionColor", Color.green);
            light.color = Color.green;
        }
        if (!connect && prevConnect)
        {
            shade.SetColor("_EmissionColor", Color.red);
            light.color = Color.red;
        }
        prevConnect = connect;
    }

    private void OnTriggerEnter(Collider collider)
    {
        Debug.Log("Reconnect trigger");
        script.connectSocket();
    }
}
