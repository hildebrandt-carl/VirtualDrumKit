using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ThemeButton : MonoBehaviour {
    public int theme = 0;
    private ChangeTheme script;

    private WebConnection web;
    Transform boxCollider;

    // Use this for initialization
    void Start () {
        this.web = GameObject.Find("Ground").GetComponent<WebConnection>();
        script = GameObject.Find("ThemeMenu").GetComponent<ChangeTheme>();
        this.boxCollider = this.transform.GetChild(0);
    }

    // Update is called once per frame
    private void OnTriggerEnter(Collider collider)
    {
        Debug.Log("Theme button trigger");
        script.setTheme(theme);
        if (web.DEBUG)
        {
            Debug.Log("OnTriggerEnter!");
            this.transform.position = this.transform.position + new Vector3(0, -0.05f, 0);
            boxCollider.position = this.transform.position + new Vector3(0, 0.05f, 0);
        }
    }

    private void OnTriggerExit(Collider other)
    {
        if (web.DEBUG)
        {
            this.transform.position = this.transform.position + new Vector3(0, 0.05f, 0);
            boxCollider.position = this.transform.position;
        }
    }
}
