//Dec 16 08:33//请不要修改此行

#include "stdafx.h"

BM2F_ENTERACE(qmtiz4_inq);

int f_qmtiz4_inq(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{
	//系统日志类定义
	CTracer log(__FUNCTION__);	

	//程序用变量
	int  doFlag = 0;
	CString q_mat_no = " ";
	CString q_order_no = " ";
	CString q_program_name = " ";
	CString q_rec_create_time_from = " ";
	CString q_rec_create_time_to = " ";
	CString q_prod_class_code = " ";
	CString q_pre_check_code =" ";

	try
	{
		//接收从前台传入的值
		q_mat_no    = bcls_rec->Tables[0].Rows[0]["MAT_NO"].ToString().Trim();
		q_order_no    = bcls_rec->Tables[0].Rows[0]["ORDER_NO"].ToString().Trim();
		q_rec_create_time_from    = bcls_rec->Tables[0].Rows[0]["PRE_CHECK_TIME_FROM"].ToString().Trim();
		q_rec_create_time_to    = bcls_rec->Tables[0].Rows[0]["PRE_CHECK_TIME_TO"].ToString().Trim();
		q_prod_class_code    = bcls_rec->Tables[0].Rows[0]["PROD_CLASS_CODE"].ToString().Trim();
		q_pre_check_code   =bcls_rec->Tables[0].Rows[0]["PRE_CHECK_CODE"].ToString().Trim();

        //获取总条数
		CString sql_count = " SELECT count(1) "
							" FROM TQMTIZ1,TQMTIZ2 "
							 " WHERE TQMTIZ1.MAT_NO = TQMTIZ2.MAT_NO "
							 " AND TQMTIZ1.MAT_NO  LIKE @mat_no||'%'"
							 " AND TQMTIZ1.ORDER_NO  LIKE @order_no||'%' "
							 " AND TQMTIZ1.PRE_CHECK_TIME >= @rec_create_time_from || ' ' "
							 " AND TQMTIZ1.PRE_CHECK_TIME <= @rec_create_time_to || '999999' "
							 " AND TQMTIZ1.PROD_CLASS_CODE  LIKE @PROD_CLASS_CODE||'%' "
							 " AND TQMTIZ1.PRE_CHECK_CODE  LIKE @PRE_CHECK_CODE||'%' "
							;

		CDbCommand cmd(sql_count,conn);
		cmd.Parameters.Set("mat_no", q_mat_no);
		cmd.Parameters.Set("order_no", q_order_no);
		cmd.Parameters.Set("rec_create_time_from",q_rec_create_time_from);
		cmd.Parameters.Set("rec_create_time_to",q_rec_create_time_to);
		cmd.Parameters.Set("PROD_CLASS_CODE",q_prod_class_code);
		cmd.Parameters.Set("PRE_CHECK_CODE",q_pre_check_code);
	    CDecimal rc=cmd.ExecuteScalar();

		if(rc>=20000)
		{
			sprintf(s.msg,"数据量超2万，请缩小范围重新查询！");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		Log::Trace("", "", "rc[{0}]",rc);
		//把值压入RC中，传出前台
        bcls_ret->ExtendedProperties.Add("RC",rc.ToString());

		//起始-终止之间
		int nStar=(int)bcls_rec->Tables[1].Rows[0]["START"];
		int nPageSize=(int)bcls_rec->Tables[1].Rows[0]["PAGE_SIZE"];
		CString sqlstr = " SELECT TQMTIZ1.ORDER_NO,TQMTIZ1.SG_SIGN,TQMTIZ1.CUST_STD,TQMTIZ1.PONO,TQMTIZ1.REP_PONO,TQMTIZ2.* "
						 " FROM TQMTIZ1,TQMTIZ2 "
						 " WHERE TQMTIZ1.MAT_NO = TQMTIZ2.MAT_NO "
						 " AND TQMTIZ1.MAT_NO  LIKE @mat_no||'%'"
						 " AND TQMTIZ1.ORDER_NO  LIKE @order_no||'%' "
						 " AND TQMTIZ1.PRE_CHECK_TIME >= @rec_create_time_from || ' ' "
						 " AND TQMTIZ1.PRE_CHECK_TIME <= @rec_create_time_to || '999999' "
						 " AND TQMTIZ1.PROD_CLASS_CODE  LIKE @PROD_CLASS_CODE||'%' "
						 " AND TQMTIZ1.PRE_CHECK_CODE  LIKE @PRE_CHECK_CODE||'%' "
						 ;

			//压入绑定参数，""包括的第一个参数的名称与SQL语句中@后对应的变量完全一致，大小写敏感
			CDbCommand cmd1(sqlstr,conn);

			cmd1.Parameters.Set("mat_no", q_mat_no);
			cmd1.Parameters.Set("order_no", q_order_no);
			cmd1.Parameters.Set("rec_create_time_from",q_rec_create_time_from);
			cmd1.Parameters.Set("rec_create_time_to",q_rec_create_time_to);
			cmd1.Parameters.Set("PROD_CLASS_CODE",q_prod_class_code);
			cmd1.Parameters.Set("PRE_CHECK_CODE",q_pre_check_code);

		int count=cmd1.ExecuteQuery(bcls_ret->Tables[0],nStar,nPageSize);
		bcls_ret->Tables[0].set_TableName("QMTIZ4_INQ");
	Log::Trace("", "", "count[{0}]",count);
		/*.print();*/
		sprintf(s.msg,"查询成功！");

	}

	//捕获数据库操作异常
	catch(CDbException& ex)  
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg,  _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = ex.GetMsg();

		//返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg)-1);
		s.flag = -1;

		//数据库异常时返回-1，事务将被回滚
		doFlag = -1;
	}

	//try块程序发生异常
	catch(const CApplicationException& ex)
	{
		doFlag = ex.GetCode();
		sprintf(s.msg,"%s:%s",(const char*)ex.GetSource(),(const char*)ex.GetMsg());
	}

	//平台异常
	catch(const CException& ex)
	{
		doFlag = -1;
		strcpy(s.msg, (const char*)ex.GetMsg());
	}

	s.flag = doFlag;

	return(doFlag);
}
