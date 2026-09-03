/*************************************************
版权: Baosight Software LTD.co Copyright (c) 2012
作者: 任龙
日期: 2012-03-16
功能: 输入复样数据
修改历史：
	日期:________；修改人：________; 需求提出人________
	变更内容:

**************************************************/
/*<remark>=========================================================
/// <summary>
/// 输入复样数据
/// <para>
/// 1. 读取前台传入参数；
/// 2. 获取表列名；
/// 3. 建立修改语句；
/// 4. 执行修改操作，返回修改结果。
/// </para>
/// </summary>
/// <param name="TABLE_NAME">界面名称</param>
/// <param name="sql_upd">拼接sql语句</param>
/// <returns>修改基表信息</returns>
===========================================================</remark>*/


#include "stdafx.h"
#include "tqmtifa.h"


BM2F_ENTERACE(qmtifa_upd2);


int f_qmtifa_upd2(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{
    // 系统日志类定义
	CTracer log(__FUNCTION__);	

	// 程序内部变量
	int         doFlag          =   0;
    
    // 数据库SQL操作字符串，用于捕获数据库操作异常情况
	CString     sqlstr          =   "";	
	
    // 记录当前时间
	CString     datetimeNow     =   CDateTime::Now().ToString("yyyyMMddHHmmss");

	// 实体类定义 
	CTQMTIFA tqmtifa(conn);

    CDbCommand cmd(conn);

	try
	{
		//记录总条数
		int count = bcls_rec->Tables[0].Rows.get_Count();


        // 获取前台输入参数
		CString lot_no_pe             = bcls_rec->Tables[0].Rows[0]["LOT_NO_PE"].ToString(); 

		Log::Trace("","","批次号=[%s]",(const char*)lot_no_pe);

		CDecimal fe_fe2o3_tc          = bcls_rec->Tables[0].Rows[0]["FE_FE2O3_TC"].ToDecimal();
		CDecimal fe_cao_tc            = bcls_rec->Tables[0].Rows[0]["FE_CAO_TC"].ToDecimal();
		CDecimal fe_sio2_tc           = bcls_rec->Tables[0].Rows[0]["FE_SIO2_TC"].ToDecimal();
		CDecimal fe_mno_tc            = bcls_rec->Tables[0].Rows[0]["FE_MNO_TC"].ToDecimal();
		CDecimal fe_al2o3_tc          = bcls_rec->Tables[0].Rows[0]["FE_AL2O3_TC"].ToDecimal();
		CDecimal fe_cl_tc             = bcls_rec->Tables[0].Rows[0]["FE_CL_TC"].ToDecimal();
		CDecimal fe_so4_tc            = bcls_rec->Tables[0].Rows[0]["FE_SO4_TC"].ToDecimal();
		CDecimal fe_feo_tc            = bcls_rec->Tables[0].Rows[0]["FE_FEO_TC"].ToDecimal();
		CDecimal fe_water_tc          = bcls_rec->Tables[0].Rows[0]["FE_WATER_TC"].ToDecimal();
		CDecimal fe_loose_lg_tc       = bcls_rec->Tables[0].Rows[0]["FE_LOOSE_LG_TC"].ToDecimal();
		CDecimal fe_comd_tc           = bcls_rec->Tables[0].Rows[0]["FE_COMD_TC"].ToDecimal();
		CDecimal fe_grain_tc          = bcls_rec->Tables[0].Rows[0]["FE_GRAIN_TC"].ToDecimal();
		CDecimal fe_sur_tc            = bcls_rec->Tables[0].Rows[0]["FE_SUR_TC"].ToDecimal();
		CDecimal fe_b_tc              = bcls_rec->Tables[0].Rows[0]["FE_B_TC"].ToDecimal();
		CDecimal fe_tio2_tc           = bcls_rec->Tables[0].Rows[0]["FE_TIO2_TC"].ToDecimal();
		CDecimal fe_mgo_tc            = bcls_rec->Tables[0].Rows[0]["FE_MGO_TC"].ToDecimal();
		CDecimal fe_na2o_tc           = bcls_rec->Tables[0].Rows[0]["FE_NA2O_TC"].ToDecimal();
		CDecimal fe_k2o_tc            = bcls_rec->Tables[0].Rows[0]["FE_K2O_TC"].ToDecimal();
		CDecimal fe_p2o5_tc           = bcls_rec->Tables[0].Rows[0]["FE_P2O5_TC"].ToDecimal();
		CDecimal fe_nio_tc            = bcls_rec->Tables[0].Rows[0]["FE_NIO_TC"].ToDecimal();
		CDecimal fe_cr2o3_tc          = bcls_rec->Tables[0].Rows[0]["FE_CR2O3_TC"].ToDecimal();
		CDecimal fe_cuo_tc            = bcls_rec->Tables[0].Rows[0]["FE_CUO_TC"].ToDecimal();


		CString fe_fe2o3              = "";
		CString fe_cao				  = "";
		CString fe_sio2				  = "";
		CString fe_mno				  = "";
		CString fe_al2o3		      = "";	
		CString fe_cl				  = "";
		CString fe_so4				  = "";
		CString fe_feo				  = "";
		CString fe_water			  = "";
		CString fe_loose_lg			  = "";
		CString fe_comd 			  = "";
		CString fe_grain			  = "";
		CString fe_sur   			  = "";
		CString fe_b				  = "";
		CString fe_tio2				  = "";
		CString fe_mgo				  = "";
		CString fe_na2o				  = "";
		CString fe_k2o				  = "";
		CString fe_p2o5				  = "";
		CString fe_nio				  = "";
		CString fe_cr2o3			  = "";
		CString fe_cuo				  = "";

		sqlstr = " SELECT FE_FE2O3,FE_CAO,FE_SIO2,FE_MNO,FE_AL2O3,FE_CL,FE_SO4,FE_FEO, "
				 " FE_WATER,FE_LOOSE_LG,FE_COMD,FE_GRAIN,FE_SUR,FE_B,FE_TIO2,"
				 " FE_MGO,FE_NA2O,FE_K2O,FE_P2O5,FE_NIO,FE_CR2O3,FE_CUO FROM TQMTIFA WHERE LOT_NO_PE = @lot_no_pe ";

		cmd.SetCommandText(sqlstr);
		cmd.Parameters.Set("lot_no_pe", lot_no_pe);

		Log::Trace("","","试批号{0}",lot_no_pe);

		cmd.ExecuteReader();
		if (cmd.Read())
		{
			 fe_fe2o3              = cmd.GetString(1);
			 fe_cao				   = cmd.GetString(2);
			 fe_sio2			   = cmd.GetString(3);
			 fe_mno				   = cmd.GetString(4);
			 fe_al2o3		       = cmd.GetString(5);	
			 fe_cl				   = cmd.GetString(6);
			 fe_so4				   = cmd.GetString(7);
			 fe_feo				   = cmd.GetString(8);
			 fe_water			   = cmd.GetString(9);
			 fe_loose_lg		   = cmd.GetString(10);
			 fe_comd 			   = cmd.GetString(11);
			 fe_grain			   = cmd.GetString(12);
			 fe_sur   			   = cmd.GetString(13);
			 fe_b				   = cmd.GetString(14);
			 fe_tio2			   = cmd.GetString(15);
			 fe_mgo				   = cmd.GetString(16);
			 fe_na2o			   = cmd.GetString(17);
			 fe_k2o				   = cmd.GetString(18);
			 fe_p2o5			   = cmd.GetString(19);
			 fe_nio				   = cmd.GetString(20);
			 fe_cr2o3			   = cmd.GetString(21);
			 fe_cuo				   = cmd.GetString(22);
		}
		cmd.Close();

		// 设置修改信息
		tqmtifa.Reset();

        tqmtifa.LOT_NO_PE = lot_no_pe;

		if (fe_fe2o3 == "*")
		{
			tqmtifa.FE_FE2O3_TC = fe_fe2o3_tc;
			tqmtifa.Update("FE_FE2O3_TC");
		}
		if (fe_cao == "*")
		{
			tqmtifa.FE_CAO_TC = fe_cao_tc;
			tqmtifa.Update("FE_CAO_TC");
		}
		if (fe_sio2 == "*")
		{
			tqmtifa.FE_SIO2_TC = fe_sio2_tc;
			tqmtifa.Update("FE_SIO2_TC");
		}
		if (fe_mno == "*")
		{
			tqmtifa.FE_MNO_TC = fe_mno_tc;
			tqmtifa.Update("FE_MNO_TC");
		}
		if (fe_al2o3 == "*")
		{
			tqmtifa.FE_AL2O3_TC = fe_al2o3_tc;
			tqmtifa.Update("FE_AL2O3_TC");
		}
		if (fe_cl == "*")
		{
			tqmtifa.FE_CL_TC = fe_cl_tc;
			tqmtifa.Update("FE_CL_TC");
		}
		if (fe_so4 == "*")
		{
			tqmtifa.FE_SO4_TC = fe_so4_tc;
			tqmtifa.Update("FE_SO4_TC");
		}
		if (fe_feo == "*")
		{
			tqmtifa.FE_FEO_TC = fe_feo_tc;
			tqmtifa.Update("FE_FEO_TC");
		}
		if (fe_water == "*")
		{
			tqmtifa.FE_WATER_TC = fe_water_tc;
			tqmtifa.Update("FE_WATER_TC");
		}
		if (fe_loose_lg == "*")
		{
			tqmtifa.FE_LOOSE_LG_TC = fe_loose_lg_tc;
			tqmtifa.Update("FE_LOOSE_LG_TC");
		}
		if (fe_comd == "*")
		{
			tqmtifa.FE_COMD_TC = fe_comd_tc;
			tqmtifa.Update("FE_COMD_TC");
		}
		if (fe_grain == "*")
		{
			tqmtifa.FE_GRAIN_TC = fe_grain_tc;
			tqmtifa.Update("FE_GRAIN_TC");
		}
		if (fe_sur == "*")
		{
			tqmtifa.FE_SUR_TC = fe_sur_tc;
			tqmtifa.Update("FE_SUR_TC");
		}
		if (fe_b == "*")
		{
			tqmtifa.FE_B_TC = fe_b_tc;
			tqmtifa.Update("FE_B_TC");
		}
		if (fe_tio2 == "*")
		{
			tqmtifa.FE_TIO2_TC = fe_tio2_tc;
			tqmtifa.Update("FE_TIO2_TC");
		}
		if (fe_mgo == "*")
		{
			tqmtifa.FE_MGO_TC = fe_mgo_tc;
			tqmtifa.Update("FE_MGO_TC");
		}
		if (fe_na2o == "*")
		{
			tqmtifa.FE_NA2O_TC = fe_na2o_tc;
			tqmtifa.Update("FE_NA2O_TC");

		}
		if (fe_k2o == "*")
		{
			tqmtifa.FE_K2O_TC = fe_k2o_tc;
			tqmtifa.Update("FE_K2O_TC");
		}
		if (fe_p2o5 == "*")
		{
			tqmtifa.FE_P2O5_TC = fe_p2o5_tc;
			tqmtifa.Update("FE_P2O5_TC");
		}
		if (fe_nio == "*")
		{
			tqmtifa.FE_NIO_TC = fe_nio_tc;
			tqmtifa.Update("FE_NIO_TC");
		}
		if (fe_cr2o3 == "*")
		{
			tqmtifa.FE_CR2O3_TC = fe_cr2o3_tc;
			tqmtifa.Update("FE_CR2O3_TC");
		}
		if (fe_cuo == "*")
		{
			tqmtifa.FE_CUO_TC = fe_cuo_tc;
			tqmtifa.Update("FE_CUO_TC");
		}
		tqmtifa.REC_REVISOR	      =  s.userid;
		tqmtifa.REC_REVISE_TIME	  =  datetimeNow;

		//更新信息
		sqlstr = "UPDATE TQMTIFA"; 
		tqmtifa.Update(" REC_REVISOR,REC_REVISE_TIME");

	}

	/*捕获数据库操作异常*/
	catch(CDbException& ex)
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg,  _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = ex.GetMsg() + "\r\n" + sqlstr;
		/*返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应*/
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg)-1);	
		s.flag = -1;
		/*数据库异常时返回-1，事务将被回滚*/
		doFlag = -1;   
	}
	/*捕获应用错误*/
	catch(CApplicationException& ex)
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

	s.flag = doFlag;

	return(doFlag);
}
