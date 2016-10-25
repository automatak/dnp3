package com.automatak.render.dnp3.enums

import com.automatak.render._

object ChannelState {

  private val comments = List("Enumeration for possible states of a channel")
  private val defaultValue = EnumValue("SHUTDOWN", 4, "stopped and will never do anything again")

  def apply(): EnumModel = EnumModel("ChannelState", comments, EnumModel.UInt8, codes, Some(defaultValue), Base10)

  private val codes = List(
    EnumValue("CLOSED", 0, "offline and idle"),
    EnumValue("OPENING", 1, "trying to open"),
    EnumValue("WAITING", 2, "waiting to open"),
    EnumValue("OPEN", 3, "open")
  )

}



