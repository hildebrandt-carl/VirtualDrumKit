using UnityEngine;

using System;
using System.Net.Sockets;
using System.IO;

public class WebConnection : MonoBehaviour {
    String myip = "160.119.248.28";
    int port = 4242;

    TcpClient socket;
    NetworkStream stream;
    StreamWriter writer;
    StreamReader reader;
    bool socketReady;

    String msg;

    public bool TESTING = false; // set to avoid actually connecting to web socket
    public bool DEBUG = true; // print/show debug information
    
    // Set up the socket on startup
    void Start()
    {
        connectSocket();
        msg = "";
    }
	
    // Add flag for drum hit
    public void AddHit(char c)
    {
        msg = msg + c;
    }
    // Check if socket is connected
    public bool isConnected()
    {
        return socketReady;
    }
    // (Re)Connect socket
    public void connectSocket()
    {
        if (DEBUG) { Debug.Log("Setting up socket..."); }
        if (!socketReady)
        {
            setupSocket();
        }
        writeSocket("vr");
    }


    // Send message after all drums could be hit
    void LateUpdate () {
        // check for ack
        String ack = readSocket();
        if(ack == "rej")
        {
            closeSocket();// connection accepted
            if (DEBUG) { Debug.Log("connection closed by server."); }
        }
        // send data from this frame and reset for next frame
        if (msg!="")
        {
            //    writeSocket(byte1, byte2);
            writeSocket(msg);
            if (DEBUG) { Debug.Log("wrote to socket: "+msg); }
        }
        msg = "";
    }

    // Close the socket on shutdown
    private void OnDestroy()
    {
        closeSocket();
    }


    // HELPER FUNCTIONS
    private void setupSocket()
    {
        if (!TESTING)
        {
            try
            {
                socket = new TcpClient(myip, port);
                stream = socket.GetStream();
                writer = new StreamWriter(stream);
                reader = new StreamReader(stream);
                socketReady = true;
                Debug.Log("Socket connected.");
            }
            catch (Exception e)
            {
                Debug.Log("SOCKET ERROR: " + e);
            }
        }
    }
    private void writeSocket(string theLine)
    {
        if (!TESTING)
        {
            if (!socketReady) { return; }
            writer.Write(theLine);
            writer.Flush();
        }
    }
    private String readSocket()
    {
        if (!TESTING)
        {
            if (!socketReady) { return ""; }
            if (stream.DataAvailable)
            {
                return reader.ReadLine();
            }
            return "";
        }
        return "";
    }
    private void closeSocket()
    {
        if (!TESTING)
        {
            if (!socketReady) { return; }
            socketReady = false;
            writer.Close();
            reader.Close();
            socket.Close();
        }
    }
}
