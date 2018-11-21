using System;
using System.Collections;
using System.Collections.Generic;
using System.IO.Ports;
using UnityEngine;

public class SerialConnection<T>
{
    SerialPort sp;

    public delegate void OnReceiveData(T data);
    public event OnReceiveData ReceiveData;

    private List<char> buffer = new List<char>();

    public SerialConnection(string device, int baudRate)
    {
        try
        {
            Debug.Log("Serial Ports: " + string.Join(", ", SerialPort.GetPortNames()));
            Debug.Log("Connecting to: " + device);
            sp = new SerialPort(device, baudRate, Parity.None, 8, StopBits.One);
            if (sp != null)
            {
                sp.Open();
            }
            else
            {
                Debug.Log("Couldn't open the USB port");
            }
        }
        catch (Exception e)
        {
            Debug.Log(e.Message);
            sp = null;
        }
    }
    
    // Update is called once per frame
    public void Update()
    {
        if (sp != null)
        {
            T msg = default(T);
            bool recvMsg = false;
            // Keep pulling bytes until we have all the bytes in the buffer
            while (sp.BytesToRead > 0)
            {
                buffer.Add((char)sp.ReadByte());
            }

            if (buffer.Count > 0)
            {
                string strBuffer = new string(buffer.ToArray());
                string[] messages = strBuffer.Split(new char[] { '\n' });
                
                // Process second to last message only (most recent)
                if (messages.Length > 1)
                {
                    msg = JsonUtility.FromJson<T>(messages[messages.Length - 2]);
                    recvMsg = true;
                }
                // Store last message - it may be complete, but we can't be sure
                buffer = new List<char>(messages[messages.Length - 1].ToCharArray());
            }
            if (recvMsg)
            {
                ReceiveData(msg);
            }
        }
    }
}
