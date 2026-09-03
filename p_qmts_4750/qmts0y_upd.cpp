/*************************************************

Copyright:   Baosight Software LTD.co Copyright (c) 2010

Author:      冯晓轶

Version:     1.0

Date:        2012-04-11

Description: 修改工艺卡

**************************************************/



//框架公用头文件，勿删

#include "stdafx.h"

//程序用头文件，请包含在""中

#include "tqmts0y.h"

#include "hqmts0y.h"

#include "tqmts02.h"

#include "hqmts02.h"

#include "tep0002.h"



/*<remark>=========================================================

/// <summary>

/// 修改工艺卡

/// <para>

/// 1.修改工艺卡和对应的成分标准信息

/// </para>

/// <para>数据库表：TQMTS0Y(工艺卡表);

//                  TQMTS02(工序成分标准表)                </para>

/// <para>主调用函数：前台QMTS0Y画面的F4(修改)调用。       </para>

/// </summary>

/// <param name="SG_CODE"> 出钢记号                          </param>

===========================================================</remark>*/



/* ***** 外部函数申明 ***** */

//int f_pssm_stno_chk_using(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn); //校验出钢记号是否在计划中使用



// service入口

BM2F_ENTERACE(qmts0y_upd)



int f_qmts0y_upd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn)

{

	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义



	/*程序用变量*/

	int doFlag = 0;

	int i = 0;

	int fetchRowCount = 0;



	CDecimal flag = 0;

	CDecimal i_count = 0;



	EIClass bcls_rec_s;

	bcls_rec_s.Tables[0].Columns.Add(DT_STRING,"FACTORY_DIV");

	bcls_rec_s.Tables.Add();

	bcls_rec_s.Tables[1].Columns.Add(DT_STRING,"ST_IDX_NO");



	/* 实体类定义 */

	CTQMTS0Y tqmts0y(conn);

	CHQMTS0Y hqmts0y(conn);

	CTQMTS02 tqmts02(conn);

	CHQMTS02 hqmts02(conn);

	CTEP0002 tep0002(conn);



	CString sqlstr("");



	/* 数据库操作类定义：统一放在Service或函数前段 */

	CDbCommand cmd_inq(conn);



	try

	{

		/*获得传入参数*/


		tqmts0y.SG_CODE = bcls_rec->Tables[0].Rows[0]["SG_CODE"].ToString().Trim();


		
		tqmts0y.HEAT_ELM_IDX = bcls_rec->Tables[0].Rows[0]["HEAT_ELM_IDX"].ToString().Trim();




		Log::Trace("", "","qmts0y_upd IN:---SG_CODE = [{0}]",(const char*)tqmts0y.SG_CODE);



		//校验出钢记号是否已审核——add by 冯晓轶 2012-04-26

		switch(conn->DatabaseKind)

		{

		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）

		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）

		case DB_KIND_MSSQL:				// MS SQL Server数据库

		case DB_KIND_ORACLE:	        // Oracle 数据库

		default:						// 所有数据库适用，通用SQL语句

			sqlstr = " SELECT VALID_FLAG "

				"   FROM TQMTS0Y "						   

				"  WHERE SG_CODE = @tqmts0y.SG_CODE "
				"  AND HEAT_ELM_IDX = @tqmts0y.HEAT_ELM_IDX";

			break;

		}

		cmd_inq.SetCommandText(sqlstr);


		cmd_inq.Parameters.Set("tqmts0y.SG_CODE", tqmts0y.SG_CODE);
		
		cmd_inq.Parameters.Set("tqmts0y.HEAT_ELM_IDX", tqmts0y.HEAT_ELM_IDX);

		cmd_inq.ExecuteReader();

		if(cmd_inq.Read())

		{

			tqmts0y.VALID_FLAG = cmd_inq.GetString(1);

		}

		cmd_inq.Close();

		if(tqmts0y.VALID_FLAG == "1")

		{

			strcpy(s.msg,_RES("QM00S0005918")/*该出钢记号已审核，不可修改。*/);

			throw CApplicationException(-1, s.msg, log.Location);

		}



		////调用检查出钢记号是否在计划中使用的函数——add by 冯晓轶 2012-03-23

		//bcls_rec_s.Tables[0].Rows.Add();

		//bcls_rec_s.Tables[0].Rows[0]["FACTORY_DIVH"] = tqmts0y.FACTORY_DIV;

		//bcls_rec_s.Tables[1].Rows.Add();

		//bcls_rec_s.Tables[1].Rows[0]["ST_IDX_NO"] = tqmts0y.SG_CODE;



		//doFlag = f_pssm_stno_chk_using(&bcls_rec_s, bcls_ret,conn);



		//if(doFlag != 0)

		//{

		//	Log::Trace("", "","f_pssm_stno_chk_using REEOR");

		//	throw CApplicationException(-1, s.msg, log.Location);

		//}


			//写工艺卡历史表——add by 冯晓轶 2012-03-23

			Log::Trace("", "","INSERT HQMTS0Y");

			switch(conn->DatabaseKind)

			{

			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）

			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）

			case DB_KIND_MSSQL:				// MS SQL Server数据库

			case DB_KIND_ORACLE:	        // Oracle 数据库

			default:						// 所有数据库适用，通用SQL语句

				sqlstr = " SELECT * "

					"   FROM TQMTS0Y "						   


					"  WHERE SG_CODE = @tqmts0y.SG_CODE "
				
					"  AND HEAT_ELM_IDX = @tqmts0y.HEAT_ELM_IDX ";

				break;

			}

			cmd_inq.SetCommandText(sqlstr);


			cmd_inq.Parameters.Set("tqmts0y.SG_CODE", tqmts0y.SG_CODE);
			
			cmd_inq.Parameters.Set("tqmts0y.HEAT_ELM_IDX", tqmts0y.HEAT_ELM_IDX);

			cmd_inq.ExecuteReader();

			if(cmd_inq.Read())

			{

				cmd_inq.Fetch(hqmts0y);

			}

			cmd_inq.Close();

			hqmts0y.DU_FLAG = "U";

			hqmts0y.DU_MAKER = s.userid;

			hqmts0y.DU_TIME = CDateTime::Now().ToString("yyyyMMddHHmmss");

			hqmts0y.TrimOrBlank();
			Log::Trace("", "","INSERT HQMTS0Y");

			hqmts0y.Insert();



			//删除工艺卡表

			Log::Trace("", "","DELETE TQMTS0Y");

			tqmts0y.Delete("SG_CODE, HEAT_ELM_IDX");



			//新增工艺卡内容

			Log::Trace("", "","INSERT TQMTS0Y");

			tqmts0y.Reset();

			tqmts0y.MergeFrom(bcls_rec->Tables[0].Rows[0]);



			if(tqmts0y.PUR_TEMP_MIN > tqmts0y.PUR_TEMP_MAX)

			{

					strcpy(s.msg,"浇铸温度下限不能大于浇铸温度上限。");

					throw CApplicationException(-1, s.msg, log.Location);

			}

			if(tqmts0y.CAST_SPEED_MIN > tqmts0y.CAST_SPEED_MAX)

			{

					strcpy(s.msg,"浇铸速度下限不能大于浇铸速度上限。");

					throw CApplicationException(-1, s.msg, log.Location);

			}

			if(tqmts0y.REFINE_TIME_MIN > tqmts0y.REFINE_TIME_MAX)

			{

					strcpy(s.msg,"精炼时间下限不能大于精炼时间上限。");

					throw CApplicationException(-1, s.msg, log.Location);

			}

			if(tqmts0y.REFINE_TEMP_MIN > tqmts0y.REFINE_TEMP_MAX)

			{

					strcpy(s.msg,"精炼温度下限不能大于精炼温度上限。");

					throw CApplicationException(-1, s.msg, log.Location);

			}

			if(tqmts0y.SMELT_TIME_MIN > tqmts0y.SMELT_TIME_MAX)

			{

					strcpy(s.msg,"冶炼时间下限不能大于冶炼时间上限。");

					throw CApplicationException(-1, s.msg, log.Location);

			}

			if(tqmts0y.SMELT_TEMP_MIN > tqmts0y.SMELT_TEMP_MAX)

			{

					strcpy(s.msg,"冶炼温度下限不能大于冶炼温度上限。");

					throw CApplicationException(-1, s.msg, log.Location);

			}





			/******************** 赋初值 *********************/

			tqmts0y.REC_CREATOR = hqmts0y.REC_CREATOR;

			tqmts0y.REC_CREATE_TIME = hqmts0y.REC_CREATE_TIME;

			tqmts0y.REC_REVISOR = s.userid;

			tqmts0y.REC_REVISE_TIME = CDateTime::Now().ToString("yyyyMMddHHmmss");

			tqmts0y.VERSION = hqmts0y.VERSION + 1;//修改一次，版本加1



			tqmts0y.VALID_FLAG = " ";

			tqmts0y.CHECK_TIME = " ";

			tqmts0y.CHECK_MAKER = " ";



			tqmts0y.TrimOrBlank();

			tqmts0y.Insert();


	}

	catch(CDbException& ex)  //捕获数据库操作异常

	{

		CFormattable arguments[] = { ex.GetCode() };

		CMessageFormat::Format(s.msg,  _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);

		CString str = sqlstr + "\r\n" + ex.GetMsg();

		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg)-1);

		s.flag = -1;

		doFlag = -1;      //数据库异常时返回-1，事务将被回滚

	}

	catch(CApplicationException& ex)  //捕获应用错误

	{

		s.flag = ex.GetCode();

		doFlag = -1;

	}

	catch(CException& ex)

	{

		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg)-1);

		s.flag = ex.GetCode();

		doFlag = -1;

	}



	return doFlag;

}

