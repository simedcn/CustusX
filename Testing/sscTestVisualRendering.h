#ifndef SSCTESTVISUALRENDERING_H_
#define SSCTESTVISUALRENDERING_H_


/**Unit tests for class ssc::VisualRendering
 */
class TestVisualRendering : public CppUnit::TestFixture
{
public:
	void setUp();
	void tearDown();
	void testInitialize();
	void testEmptyView();
	void test_3D_Tool();
	void test_3D_Tool_GPU();
	void test_ACS_3D_Tool();
	void test_AnyDual_3D_Tool();
	void test_ACS_3Volumes();
	void test_AnyDual_3Volumes();
	void test_ACS_3Volumes_GPU();
	
public:
	CPPUNIT_TEST_SUITE( TestVisualRendering );
		CPPUNIT_TEST( testInitialize );					
		CPPUNIT_TEST( testEmptyView );					
		CPPUNIT_TEST( test_3D_Tool );					
		CPPUNIT_TEST( test_3D_Tool_GPU );
		CPPUNIT_TEST( test_ACS_3D_Tool );					
		CPPUNIT_TEST( test_AnyDual_3D_Tool );					
		CPPUNIT_TEST( test_ACS_3Volumes );					
		CPPUNIT_TEST( test_AnyDual_3Volumes );					
		CPPUNIT_TEST( test_ACS_3Volumes_GPU );
	CPPUNIT_TEST_SUITE_END();
private:
	class ViewsWindow* widget;
	std::vector<QString> image;
	bool runWidget();
};

CPPUNIT_TEST_SUITE_REGISTRATION( TestVisualRendering );

#endif /*TESTVISUALRENDERING_H_*/
