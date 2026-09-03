/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   chenwenqiong
Version:    1.0
Date:     2011-08-12 17:13:56
Description: 获取炼钢计划所需的制造标准数据
**************************************************/

#include "stdafx.h"



/*<remark>=========================================================
/// <summary>
///获取炼钢计划所需的制造标准数据（MMS层用函数）
/// <para>
/// 材料申请的命令提交，写入炼钢计划的命令表（TPSSM01/02/03）时，读取质量的制造标准数据。
/// 并做相应的质量数据校验。
/// </para>
/// <para>数据库表：TQMTS0X(工艺卡)
/// <para>主调用函数：该接口函数在材料申请下达给炼钢计划时调用
/// <para>需调用函数： 
/// </summary>
/// <param name="ST_NO">出钢记号             </param>
/// <returns>  </returns>
===========================================================</remark>*/
BM2_FUNCTION_EXPORT
 int f_qmts_stno_inq_01(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	//变量声明
	int  doFlag = 0;
	int  blknum = 0;

	CString ds_flag = " ";
	CString  smelt_div = " "; //冶炼区分：B-转炉; E-电炉
	CDecimal smelt_mode = 0;  //冶炼模式：1-单联; 2-双联
	CString  refine_div = ""; //精炼区分
	CString  ic_cc_flag = ""; //模连铸区分


	CString sqlstr("");
	/* 实体类定义 */
	CModel tqmts0x("TQMTS0X");

	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);


	try
	{
		//--------------------------------------
		//定义返回数据块
		if(bcls_ret->Tables.Contains("TQMTS0X") == false)  //炼钢工艺数据
		{
			CDataTable& table = bcls_ret->Tables.Add("TQMTS0X");
			//table.Columns.Add(tmmsm96);
		}
		blknum = bcls_ret->Tables.IndexOf("TQMTS0X");
		Log::Trace("", __FUNCTION__, "blknum=[{0}]", blknum);

		if(bcls_ret->Tables[blknum].Columns.Contains("BACKLOG_EA") == false)
		{
			bcls_ret->Tables[blknum].Columns.Add(DT_STRING, "BACKLOG_EA");    //钢区工艺途径：整个炼钢计划及调度计划用的参数，必须准确。
		}
		if(bcls_ret->Tables[blknum].Columns.Contains("SMELT_DIV") == false)
		{
			bcls_ret->Tables[blknum].Columns.Add(DT_STRING, "SMELT_DIV");     //冶炼区分：B-转炉; E-电炉 （必须有）
		}
		if(bcls_ret->Tables[blknum].Columns.Contains("SMELT_MODE") == false)
		{
			bcls_ret->Tables[blknum].Columns.Add(DT_DECIMAL,"SMELT_MODE");    //冶炼模式：1-单联; 2-双联 （默认1）
		}
		if(bcls_ret->Tables[blknum].Columns.Contains("SRP_DIV") == false)
		{
			bcls_ret->Tables[blknum].Columns.Add(DT_STRING, "SRP_DIV");       //精炼区分：允许空
		}
		if(bcls_ret->Tables[blknum].Columns.Contains("CC_SLAG") == false)
		{
			bcls_ret->Tables[blknum].Columns.Add(DT_STRING, "CC_SLAG");       //连铸机采用。 炼钢计划用
		}
		if(bcls_ret->Tables[blknum].Columns.Contains("SLAB_DERP_REQ") == false)
		{
			bcls_ret->Tables[blknum].Columns.Add(DT_STRING, "SLAB_DERP_REQ"); //轻压下要求, 取质量的数据
		}
		if(bcls_ret->Tables[blknum].Columns.Contains("SLAB_COOL_IND") == false)
		{
			bcls_ret->Tables[blknum].Columns.Add(DT_STRING, "SLAB_COOL_IND"); //板坯缓冷指示, 取质量的数据
		}
		if(bcls_ret->Tables[blknum].Columns.Contains("CC_BASE") == false)
		{
			bcls_ret->Tables[blknum].Columns.Add(DT_STRING, "CC_BASE");       //连连铸基准, 取质量的数据，校验计划准确性。
		}


		//--------------------------------------
		/*获得传入参数*/
		tqmts0x["FACTORY_DIV"] = bcls_rec->Tables[0].Rows[0]["FACTORY_DIV"].ToString().TrimOrBlank();
		tqmts0x["ST_NO"]		= bcls_rec->Tables[0].Rows[0]["ST_NO"].ToString().Trim();

		Log::Trace("", __FUNCTION__, "st_no=[{0}], sm_unit_no=[{1}]", tqmts0x["ST_NO"].ToString(), tqmts0x["FACTORY_DIV"].ToString());
				
		//if(tqmts0x["FACTORY_DIV"].ToString().TrimOrBlank() == " ")
		//{
		//	tqmts0x["FACTORY_DIV"] = "A";//如果厂别为空，不报错，默认为A;//(备注:出钢记号浦钢的厂别为”A”，其他项目组需要修改)。
		//}

		if (tqmts0x["ST_NO"].ToString().TrimOrBlank() == " ")
		{
			strcpy(s.msg,_RES("QM00S0004171")/*出钢记号不可为空。*/);
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//定义数据库操作命令对象comm_inq执行sql语句，sql字符串用""包括，可以分行，但每行前后务必留出一个空格。
		switch(conn->DatabaseKind)
		{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:						// 所有数据库适用，通用SQL语句
				sqlstr = " SELECT SMELT_DIV, "
						 "        SMELT_MODE, "
						 "        IC_CC_FLAG, "
						 "        PRESS_USE_CODE, "
						 "        SLAB_COOL_IND, "
						 "        CC_SLAG, "
						 "        REFINE_ROUTE_CODE "
						 "   FROM TQMTS0X "
						 "  WHERE ST_NO       = @tqmts0x.ST_NO "
						 "    AND (FACTORY_DIV = @tqmts0x.FACTORY_DIV "
						 "   OR  FACTORY_DIV = ' ')" ;

				break;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("tqmts0x.ST_NO", tqmts0x["ST_NO"].ToString());
		cmd_inq.Parameters.Set("tqmts0x.FACTORY_DIV", tqmts0x["FACTORY_DIV"].ToString());
		cmd_inq.ExecuteReader();
		if(cmd_inq.Read())
		{
			tqmts0x["SMELT_DIV"]      = cmd_inq.GetString(1);
			tqmts0x["SMELT_MODE"]     = cmd_inq.GetDecimal(2);
			tqmts0x["IC_CC_FLAG"]     = cmd_inq.GetString(3);
			tqmts0x["PRESS_USE_CODE"] = cmd_inq.GetString(4);
			tqmts0x["SLAB_COOL_IND"]  = cmd_inq.GetString(5);
			tqmts0x["CC_SLAG"]        = cmd_inq.GetString(6);
			tqmts0x["REFINE_ROUTE_CODE"] = cmd_inq.GetString(7);
		}
		cmd_inq.Close();

		smelt_div  = tqmts0x["SMELT_DIV"].ToString().TrimOrBlank();  //冶炼区分
		smelt_mode = tqmts0x["SMELT_MODE"];
		refine_div = tqmts0x["REFINE_ROUTE_CODE"].ToString().TrimOrBlank();
		ic_cc_flag = tqmts0x["IC_CC_FLAG"].ToString().TrimOrBlank();


		Log::Trace("", __FUNCTION__, "tqmts0x：SMELT_DIV=[{0}], SMELT_MODE=[{1}], REFINE_ROUTE_CODE=[{2}], ic_cc_flag=[{3}]",
			tqmts0x["SMELT_DIV"].ToString(), tqmts0x["SMELT_MODE"].ToDecimal(), refine_div, ic_cc_flag);


		//-----------------------------------------------------
		//钢区工序途径的生成，很重要。根据工艺卡内容决定炼钢生产路径。xuwen
		CString backlog_ea = "";
		
		//1.脱硫工序
		if (ds_flag == "1") //有脱硫指示，工艺卡中无此设置。待定
		{
			backlog_ea += "S";  //脱S
		}

		//2.转炉双联
		if(smelt_div == "B" && smelt_mode == 2)
		{
			backlog_ea += "P";  //脱P
		}
		else
		{
			//电炉或非双联的，都为单联法
			smelt_mode == 1;
		}

		//3.冶炼区分
		if(smelt_div == "B")
		{
			backlog_ea += "B";  //转炉
		}
		else if(smelt_div == "E")
		{
			backlog_ea += "E";  //电炉
		}
		else
		{
			//报错
			CFormattable arguments[] = {tqmts0x["ST_NO"].ToString()};
			CMessageFormat::Format(s.msg, _RES("QM00S0006068")/*出钢记号[{0}]中没有设置冶炼区分。*/, arguments, 1);
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//4.拼接精炼区分。可为空，空时跳过。
		if ( refine_div != " " && refine_div != "0000") //精炼路径非空时, 拼接
		{
			backlog_ea += refine_div;
		}
		else  //不做精炼时,统一置为空
		{
			refine_div = " ";
			tqmts0x["REFINE_ROUTE_CODE"] = refine_div;  //精炼路径应与backlog_ea中的精炼部分一致。
		}

		if (ic_cc_flag == " ")//为空，报错？
		{
			ic_cc_flag = "C"; //强制为连铸
		}
		tqmts0x["IC_CC_FLAG"] = ic_cc_flag;

		//5.拼接连铸工序
		backlog_ea += ic_cc_flag;


		//--------------------------------------------------
		//参数返回
		bcls_ret->Tables["TQMTS0X"].Rows.Clear();
		CDataRow& row = bcls_ret->Tables["TQMTS0X"].Rows.Add();
		row["BACKLOG_EA"]    = backlog_ea;
		row["SMELT_DIV"]     = tqmts0x["SMELT_DIV"];
		row["SMELT_MODE"]    = tqmts0x["SMELT_MODE"];
		row["SRP_DIV"]       = tqmts0x["REFINE_ROUTE_CODE"];
		row["CC_SLAG"]       = tqmts0x["CC_SLAG"];
		row["SLAB_DERP_REQ"] = tqmts0x["PRESS_USE_CODE"];
		row["SLAB_COOL_IND"] = tqmts0x["SLAB_COOL_IND"];
		row["CC_BASE"]       = tqmts0x["CC_BASE"];

		Log::Trace("", __FUNCTION__, "返回参数钢区工艺路径      backlog_ea[{0}]", backlog_ea);
		Log::Trace("", __FUNCTION__, "返回参数精炼区分   refine_route_code[{0}]", refine_div);
		Log::Trace("", __FUNCTION__, "返回参数冶炼区分           smelt_div[{0}]", tqmts0x["SMELT_DIV"].ToString());
		Log::Trace("", __FUNCTION__, "返回参数冶炼模式          smelt_mode[{0}]", smelt_mode );
		Log::Trace("", __FUNCTION__, "返回参数连铸机采用           cc_slag[{0}]", tqmts0x["CC_SLAG"].ToString());
		Log::Trace("", __FUNCTION__, "返回参数轻压下采用代码 SLAB_DERP_REQ[{0}]", tqmts0x["PRESS_USE_CODE"].ToString());
		Log::Trace("", __FUNCTION__, "返回参数板坯缓冷指示   slab_cool_ind[{0}]", tqmts0x["SLAB_COOL_IND"].ToString());
		Log::Trace("", __FUNCTION__, "返回参数连铸基准             CC_BASE[{0}]", tqmts0x["CC_BASE"].ToString());


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

