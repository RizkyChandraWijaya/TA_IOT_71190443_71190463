package id.ac.ukdw.firebaaseiot

import com.google.firebase.database.IgnoreExtraProperties

@IgnoreExtraProperties
data class Gate(val jarak: Int?=null , val statusGate: String?=null)

