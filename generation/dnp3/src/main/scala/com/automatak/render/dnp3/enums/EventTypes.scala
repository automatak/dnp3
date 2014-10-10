package com.automatak.render.dnp3.enums

import com.automatak.render.{EnumValue, Base10, EnumValues, EnumModel}

object EventTypes {

  private def create(name: String, values: List[EnumValue]): EnumModel = EnumModel(name, Nil, EnumModel.Integer, values, Base10)

  private def group(i: Int)(vars: Int*): List[EnumValue] = EnumValues.from(vars.map(v => List("Group",i.toString,"Var", v.toString).mkString).toList)

  // defined using syntactic sugar above
  def enums : List[EnumModel] = List(

    create("StaticBinaryResponse", group(1)(2)),
    create("StaticDoubleBinaryResponse", group(3)(2)),
    create("StaticAnalogResponse", group(30)(1,2,3,4,5,6)),
    create("StaticCounterResponse", group(20)(1,2,5,6)),
    create("StaticFrozenCounterResponse", group(21)(1,2,5,6,7,8,9,10)),
    create("StaticBinaryOutputStatusResponse",group(10)(2)),
    create("StaticAnalogOutputStatusResponse",group(40)(1,2,3,4)),
    create("StaticTimeAndIntervalResponse", group(50)(4)),


    create("EventBinaryResponse", group(2)(1,2,3)),
    create("EventDoubleBinaryResponse", group(4)(1,2,3)),
    create("EventAnalogResponse", group(32)(1,2,3,4,5,6,7,8)),
    create("EventCounterResponse", group(22)(1,2,5,6)),
    create("EventFrozenCounterResponse", group(23)(1,2,5,6)),
    create("EventBinaryOutputStatusResponse",group(11)(1,2)),
    create("EventAnalogOutputStatusResponse",group(42)(1,2,3,4,5,6,7,8))
  )
}
