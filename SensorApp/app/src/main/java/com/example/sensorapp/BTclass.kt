package com.example.userapp

import android.bluetooth.BluetoothAdapter
import android.bluetooth.BluetoothDevice
import android.bluetooth.BluetoothSocket
import android.content.Context
import android.os.Bundle
import android.os.Handler
import android.provider.SyncStateContract
import android.util.Log
import androidx.annotation.MainThread
import java.io.IOException
import java.io.InputStream
import java.io.OutputStream
import java.lang.Exception
import java.util.*

var mState = 0

var mConnectThread: Bluetooth.ConnectThread? = null
var mConnectedThread: Bluetooth.ConnectedThread? = null

class Bluetooth(context: Context?, handler: Handler?){
    //Debugging
    private val TAG: String =  "BluetoothService"
    private val D = true

    var mHandler = handler

    //Unique UUID
    val MY_UUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB") //distant device UUID

    //State messages
    val MESSAGE_STATE_CHANGE = 1
    val MESSAGE_READ = 2
    val MESSAGE_WRITE = 3
    val MESSAGE_DEVICE_NAME = 4
    val MESSAGE_TOAST = 5
    val MESSAGE_LOSTCO = 667
    val MESSAGE_FAILEDCO = 668

    // Constants that indicate the current connection state
    val STATE_NONE = 0 // we're doing nothing
    val STATE_CONNECTING = 2 // now initiating an outgoing connection
    val STATE_CONNECTED = 3 // now connected to a remote device

    @Synchronized
    private fun setState(state: Int): Int{
        if (D) Log.d(TAG, "setState() $mState -> $state")
        mState = state
        mState = getState()

        // Give the new state to the Handler so the UI Activity can update
        mHandler!!.obtainMessage(MESSAGE_STATE_CHANGE, state, -1)?.sendToTarget()
        return state
    }


    @Synchronized
    fun getState(): Int {
        Log.i(TAG, "getState(): $mState")
        return mState
    }

    fun connect(device: BluetoothDevice?): Int {
        Log.i(TAG, "----FUNCTION CONNECT:----")
        if (D) Log.d(TAG, "connect to: $device")
        getState()
        // Cancel any thread attempting to make a connection
        if (mState == STATE_CONNECTING) {
            if (mConnectThread != null) {
                Log.i(TAG, "STATE_CONNECTING: need to close the socket before continuing")
                mConnectThread?.cancel()
                mConnectThread = null
                mState = STATE_NONE
                Log.i(TAG, "Set State: $mState")
            }
        }

        // Start the thread to connect with the given device
        Log.i(TAG, "Initiating ConnectThread")
        ConnectThread(device).start()
        Log.i(TAG, "ConnectThread started")
        setState(STATE_CONNECTING)
        Log.i(TAG, "----END FUNCTION CONNECT:----")
        return mState
    }

    fun connected(socket: BluetoothSocket?, device: BluetoothDevice?, socketType: String?){
        Log.i(TAG, "----FUNCTION CONNECTED:----")
        getState()

        if (D) Log.d(TAG, "Socket Type:$socketType")

        // Cancel the thread that completed the connection
        if (mConnectThread != null) {
            mConnectThread!!.cancel()
            mConnectThread = null
        }

        // Cancel any thread currently running a connection
        if (mConnectedThread != null) {
            mConnectedThread!!.cancel()
            mConnectedThread = null
        }

        // Start the thread to manage the connection and perform transmissions
        Log.i(TAG, "Risque de plantage 1")
        mConnectedThread = ConnectedThread(socket!!)
        Log.i(TAG, "Risque de plantage 2")
        mConnectedThread!!.start()

        // Send the name of the connected device back to the UI Activity
        val msg = mHandler?.obtainMessage(MESSAGE_DEVICE_NAME)
        val bundle = Bundle()
        bundle.putString("Connected", device?.name)
        msg?.data = bundle
        mHandler?.sendMessage(msg!!)
        setState(STATE_CONNECTED)
        Log.i(TAG, "FUNCTION CONNECTED END: GET STATE")
        getState()
    }



    inner class ConnectThread(mmDevice: BluetoothDevice?) : Thread() {
        private val mmmDevice = mmDevice
        private val mmSocket = mmDevice?.createRfcommSocketToServiceRecord(MY_UUID)
        private val mSocketType: String? = null

        override fun run() {
            Log.i(TAG, "###ConnectThread run() BEGIN###")

            Log.i(TAG, "BEGIN mConnectThread SocketType:$mSocketType")
            name = "ConnectThread$mSocketType"

            // Make a connection to the BluetoothSocket
            try {
                // This is a blocking call and will only return on a
                // successful connection or an exception
                mmSocket!!.connect()
            } catch (e: IOException) {
                Log.e(TAG, "Unable to connect socket ", e)
                // Close the socket
                try {
                    Log.i(TAG, "Closing the socket")
                    mmSocket!!.close()
                    connectionFailed()
                } catch (e2: IOException) {
                    Log.e(
                        TAG, "unable to close() " + mSocketType
                                + " socket during connection failure", e2
                    )
                }
                return
            }
            Log.i(TAG, "connect() done")

            Log.i(TAG, "Current State: $mState")


            //Reset the ConnectThread because we're done
            synchronized (this@Bluetooth) {
                mConnectThread = null;
            }
            Log.i(TAG, "###ConnectThread END###")

            // Start the connected thread
            Log.i(TAG, "Start ConnectedThread")
            connected(mmSocket, mmmDevice, mSocketType)

        }

        fun cancel() {
            try {
                mmSocket!!.close()
                Log.e(TAG, "ConnectThread: SOCKET CLOSED")
            } catch (e: IOException) {
                Log.e(
                    TAG, "close() of connect " + mSocketType
                            + " socket failed", e
                )
            }
        }
    }

    inner class ConnectedThread(private val mmSocket: BluetoothSocket) : Thread() {

        private val mmInStream: InputStream = mmSocket.inputStream
        private val mmOutStream: OutputStream = mmSocket.outputStream
        private val mmBuffer: ByteArray = ByteArray(1024) // mmBuffer store for the stream

        override fun run() {
            Log.i(TAG, "BEGIN mConnectedThread")
            val buffer = ByteArray(1000)
            var bytes: Int

            // Keep listening to the InputStream while connected
            while (true) {
                try {
                    // Read from the InputStream
                    Log.i(TAG, "### Message received ###")
                    Log.e(TAG, "### WARNING 1 ###")
                    bytes = mmInStream.read(buffer)
                    Log.e(TAG, "### bytes = read(buffer) ###")
                    Log.d(TAG, "message bytes $bytes")
                    Log.d(TAG, "message buffer " + String(buffer))

                    //restrict the message to send to get rid of padding
                    val msgtosend = String(buffer).subSequence(0,bytes)

                    // Send the obtained bytes to the UI Activity
                    mHandler!!.obtainMessage(MESSAGE_TOAST, msgtosend)
                    var incoming_msg = mHandler!!.obtainMessage(MESSAGE_READ, bytes, -1, msgtosend)
                    incoming_msg.sendToTarget()

                    //mHandler!!.obtainMessage(MESSAGE_TOAST, msgtosend).sendToTarget()
                    //mHandler!!.obtainMessage(MESSAGE_TOAST, bytes, -1, String(buffer)).sendToTarget()

                } catch (e: IOException) {
                    Log.e(TAG, "disconnected", e)
                    // Start the service over to restart listening mode
                    connectionLost()
                    break
                }
            }
        }

        // Call this from the main activity to send data to the remote device.
        fun write(bytes: ByteArray) {
            try {
                mmOutStream.write(bytes)
            } catch (e: IOException) {
                Log.e(TAG, "Error occurred when sending data", e)
                // Send a failure message back to the activity.
                val writeErrorMsg = mHandler?.obtainMessage(MESSAGE_TOAST)
                val bundle = Bundle().apply {
                    putString("toast", "Couldn't send data to the other device")
                }
                writeErrorMsg?.data = bundle
                mHandler?.sendMessage(writeErrorMsg!!)
                return
            }

            // Share the sent message with the UI activity.
            val writtenMsg = mHandler?.obtainMessage(
                MESSAGE_WRITE, -1, -1, mmBuffer)
            writtenMsg?.sendToTarget()
        }

        // Call this method from the main activity to shut down the connection.
        fun cancel() {
            try {
                mmSocket.close()
            } catch (e: IOException) {
                Log.e(TAG, "Could not close the connect socket", e)
            }
        }
    }

    private fun connectionLost() {
        // Send a failure message back to the Activity
        setState(STATE_NONE)
        val msg = mHandler!!.obtainMessage(MESSAGE_LOSTCO)
        val bundle = Bundle()
        bundle.putString("Toast", "Device connection was lost")
        msg.setData(bundle)
        mHandler!!.sendMessage(msg)
    }

    private fun connectionFailed() {
        // Send a failure message back to the Activity
        stop()
        val msg = mHandler!!.obtainMessage(MESSAGE_FAILEDCO)
        val bundle = Bundle()
        bundle.putString("Toast", "Unable to connect device")
        msg.setData(bundle)
        mHandler!!.sendMessage(msg)

    }

    @Synchronized
    fun stop() {
        if (D) Log.d(TAG, "stop")
        if (mConnectThread != null) {
            mConnectThread!!.cancel()
            mConnectThread = null
        }
        if (mConnectedThread != null) {
            mConnectedThread!!.cancel()
            mConnectedThread = null
        }

        setState(STATE_NONE)
    }

    private fun write(out: ByteArray) {
        // Create temporary object
        var r = mConnectedThread
        // Synchronize a copy of the ConnectedThread
        synchronized(this) {
            //Log.i(TAG, "attention")
            if (mState != STATE_CONNECTED) return
            //Log.i(TAG, "write(): invalid state")
        }
        // Perform the write unsynchronized
        r!!.write(out)
        Log.i(TAG, "end write bytearray")
    }

    fun sendMessage(message: String) {
        // Check that we're actually connected before trying anything
        if (getState() != STATE_CONNECTED) {
            Log.w(TAG, "bluetooth is not connected")
            return
        }

        // Check that there's actually something to send
        if (message.length > 0) {
            val EOT = 3.toChar()
            // Get the message bytes and tell the BluetoothChatService to write
            val send = (message + EOT).toByteArray()
            this.write(send)
        }
    }

    fun connectDevice(deviceName: String) {
        // Get the device MAC address
        var address: String? = null
        val adapter = BluetoothAdapter.getDefaultAdapter()
        for (d in adapter.bondedDevices) {
            if (d.name == deviceName) address = d.address //If device is listed in the phone's paired devices, get the address
        }
        try {
            val device = adapter.getRemoteDevice(address) // Get the BluetoothDevice object
            connect(device) // Attempt to connect to the device
        } catch (e: Exception) {
            Log.e(TAG, "Unable to connect to $address")
        }
    }
}