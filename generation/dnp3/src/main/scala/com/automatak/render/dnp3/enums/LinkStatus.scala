package com.automatak.render.dnp3.enums

import com.automatak.render._

object LinkStatus {

  private val comments = List("Enumeration for reset/unreset states of a link layer")

  def apply(): EnumModel = EnumModel("LinkStatus", comments, EnumModel.Integer, codes, Base10)

  private val codes = List(
    EnumValue("UNRESET", 0, "Transition to UNRESET state"),
    EnumValue("RESET", 1, "Transition to RESET state"),
    EnumValue("TIMEOUT", 2, "Timeout Transition")
  )

}



