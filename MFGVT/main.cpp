#include "includes/gtkmmcomponent.hpp"


class WindowMain : public Gtk::Window , public TemplateGui
{
    public:

        WindowMain( const std::string & _absPath );

        void init(void);
        const std::string get_ressourcePath(void)const;
        
    protected :

    private:

        Gtk::HPaned m_utilitysPanel;
        Gtk::VBox m_usersPanel;
        Gtk::Notebook m_nbk;
        Prompt m_prompt;

        std::string m_absPath , m_ressourcePath;

        Glib::RefPtr< Gtk::Adjustment > m_vadj , m_hadj ;

};

WindowMain::WindowMain(const std::string & _absPath) : Gtk::Window() , TemplateGui() 
{
    this->m_absPath = _absPath;
    this->m_parent = this;
    //this->addMainPath( std::shared_ptr< const std::string >(new const std::string(_mainPath)) );

    auto && path = utilitys::readList(this->m_absPath + "\\ressourcePath.csv" , false);

    if( path.size() == 0 )
         throw std::logic_error("no ressourcePath found");

    this->m_ressourcePath = path.front();
}

const std::string WindowMain::get_ressourcePath(void)const
{
    return this->m_ressourcePath;
}

void WindowMain::init(void)
{
    this->m_prompt.addDevice(this->m_devices);
    this->m_prompt.addMainPath( this->atMainPath() );

    

    this->set_icon_from_file(  this->m_ressourcePath+ "\\version-control-lrg.png");

    this->unfullscreen();
	this->set_position(Gtk::WIN_POS_CENTER);
	this->resize(640, 480);
    this->maximize();

    this->addCssProvider( Gtk::CssProvider::create() );

    //chargement du fichier css

    std::string const cssFile =  this->m_ressourcePath + "\\style.css" ;

    try
    {
        this->atCssProvider()->load_from_path( cssFile );
    }
    catch(Gtk::CssProviderError const & e)
    {
        std::cerr << "Error : " << cssFile << " cannot be load" << std::endl;
    }

    //class pour fichier css
    this->get_style_context()->add_class("windows");
    this->m_utilitysPanel.get_style_context()->add_class("utilitysPanel");
    this->m_usersPanel.get_style_context()->add_class("userPanel");

    this->add(this->m_utilitysPanel);


    this->m_utilitysPanel.pack1( this->m_usersPanel );
    

    this->m_pdfS = std::shared_ptr<PdfShower>( new PdfShower() );

    this->m_prompt.addPdfShower( this->m_pdfS );


    this->m_utilitysPanel.pack2( *this->m_pdfS );

    this->m_usersPanel.pack_start(this->m_prompt  , Gtk::PACK_SHRINK);

    if( this->atDevice()->size() > 0 )
    {
        for(auto & cont : this->atDevice()->front().get_containers() )
        {
            cont.addVersionShower( std::shared_ptr< VersionShower>( new VersionShower() ) ) ;
            cont.atVersionShower()->addMainPath( this->atMainPath());
            cont.atVersionShower()->addPdfShower( this->m_pdfS );
            this->m_nbk.append_page( *cont.atVersionShower() ,  *Gtk::manage(new Gtk::Label( cont.get_name() )) );
        }

        //propage sur tout les device mes shower
        for(auto & dev : *this->atDevice() )
        {
            for(size_t i = 0 ; i < dev.get_containers().size() ; i++ )
            {
                dev.get_containers().at(i).addVersionShower( this->atDevice()->front().get_containers().at(i).atVersionShower() ) ;
                dev.get_containers().at(i).atVersionShower()->addPdfShower( this->m_pdfS );
            }
        }
    }

    this->m_nbk.set_scrollable();
    this->m_nbk.popup_enable();
    
    this->m_usersPanel.pack_start( this->m_nbk );
}


int main(int argc, char *argv[]) 
{
    auto progmPath = utilitys::sep_sub_and_name(std::string(argv[0]));

    Gtk::Main app(argc, argv);

    try
    {
         WindowMain fenetre(std::get<0>(progmPath));

	    fenetre.set_title("Maintenance File Grouper Version Tool");

        //! charge les donné , doit etre fait avant WindowMain::init()
        auto tload = load( fenetre.get_ressourcePath() );
        fenetre.addMainPath( std::shared_ptr< const std::string >(new const std::string(std::get<0>(tload))) );

        fenetre.addDevice( std::get<1>(tload) );
        
        for(auto & dev : *fenetre.atDevice() )
            dev.addMainPath(fenetre.atMainPath());
        
        
        fenetre.init();

        fenetre.show_all();
        
        Gtk::Main::run(fenetre);
    }
    catch(std::exception const & _e)
    {
        std::cerr << _e.what() << std::endl;

        std::cin.get();
    }
    
    

    return 0;
}

