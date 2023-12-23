#include "includes/utilitys.hpp"

#include <format>
#include <chrono>
#include <future>
#include <utility>
#include <thread>
#include <sstream>
#include <chrono>
#include <iomanip>

#include <gtkmm-3.0/gtkmm.h>






class Version
{
    public : 

        enum Error_flag : const byte{EF_CreateDate = 0b00000001, EF_ModifDate = 0b00000010  , EF_MD5 = 0b00000100 };
        Version(void);
        Version(const Version & _cpy);

        const bool cmpFile(const Version & _cpy) const ;
        const bool cmpBaseName(const Version & _cpy) const ;

        std::string get_subPathFile(void);


        const byte get_error( void )const;
        
        void add_error(Error_flag const _err);

        static const std::string VersionError( Version const & _vers);
        static std::string EF2Str(Error_flag const _err);


        std::string name ,dir ,id , createDate , modifDate, autor  , extension ,version , part; 
    protected :

    byte m_id_error;
};




Version::Version(void):m_id_error(0)
{
    
}


Version::Version(const Version & _cpy)
{
    this->dir=_cpy.dir;
    this->name=_cpy.name;
    this->id=_cpy.id;
    this->createDate=_cpy.createDate;
    this->autor=_cpy.autor;
    this->extension =_cpy.extension;
    this->version =_cpy.version;
    this->modifDate = _cpy.modifDate;
    this->part=_cpy.part;
    this->m_id_error = _cpy.get_error();
}

const bool Version::cmpFile(const Version & _cpy) const 
{
    return _cpy.dir == this->dir && _cpy.name == this->name && _cpy.extension == this->extension ;
}

const bool Version::cmpBaseName(const Version & _cpy) const 
{
    return  _cpy.name == this->name && _cpy.extension == this->extension ;
}

std::string Version::get_subPathFile(void)
{
    return this->dir + "\\" + this->name + "." +this->extension;
}

const byte Version::get_error( void) const
{
    return this->m_id_error;
}



void Version::add_error( Version::Error_flag const _err) 
{
    this->m_id_error |=  static_cast<byte>(_err);
}

const std::string Version::VersionError( Version const & _vers)
{
    std::string tmp = "";

    if( _vers.get_error() & EF_CreateDate)
    {
        tmp += EF2Str( EF_CreateDate );
        tmp+=";";
    }
        
    if( _vers.get_error() & EF_ModifDate)
    {
        tmp += EF2Str( EF_ModifDate );
        tmp+=";";
    }

    if( _vers.get_error() & EF_MD5)
    {
        tmp += EF2Str( EF_MD5 );
        tmp+=";";
    }

    return tmp;
}

std::string  Version::EF2Str(Version::Error_flag const _err)
{
    std::string tmp = "";

    switch (_err)
    {
    case EF_CreateDate:
        tmp =  "EF_CreateDate";
        break;

    case EF_ModifDate:
        tmp ="EF_ModifDate";
        break;

    case EF_MD5:
        tmp = "EF_MD5";
        break;
    
    default:
        break;
    }

    return tmp;
}

class MainPathSharedTemplate
{
    public:
        MainPathSharedTemplate(void);
        virtual ~MainPathSharedTemplate(void);

        void addMainPath(std::shared_ptr<const std::string> _mainPath);
        std::shared_ptr<const std::string> const getMainPath(void);
        std::shared_ptr<const std::string> const atMainPath(void);

    private:

    std::shared_ptr<const std::string> m_mainPath;
};

MainPathSharedTemplate::MainPathSharedTemplate(void){}
MainPathSharedTemplate::~MainPathSharedTemplate(void){}
//setter
void MainPathSharedTemplate::addMainPath(std::shared_ptr<const std::string> _mainPath)
{
    this->m_mainPath  = _mainPath;
}
//getter
std::shared_ptr<const std::string> const MainPathSharedTemplate::getMainPath(void)
{
    return this->m_mainPath;
}
//acces securisé 
std::shared_ptr<const std::string> const MainPathSharedTemplate::atMainPath(void)
{
    if(!this->m_mainPath)
        throw std::logic_error("bad mainPath acces");

    return this->m_mainPath;
}


class GrpVersion : public std::vector<Version> , public MainPathSharedTemplate
{
    public:
    GrpVersion(void);
    const std::string get_name(void) const;
    const std::string get_id(void) const;
    const std::string get_autor(void) const;
    const std::string get_createDate(void) const;
    const std::string get_modifDate(void) const;

    void merge(const GrpVersion & _grp);

    void check(void);

    private:


    
};

GrpVersion::GrpVersion(void):std::vector<Version>() , MainPathSharedTemplate()
{
    
}
const std::string GrpVersion::get_name(void) const
{
    return this->front().name;
}

const std::string GrpVersion::get_id(void) const
{
    return this->front().id;
}

const std::string GrpVersion::get_autor(void) const
{
    return this->front().autor;
}

const std::string GrpVersion::get_createDate(void) const
{
    return this->front().createDate;
}

const std::string GrpVersion::get_modifDate(void) const
{
    return this->front().modifDate;
}

void GrpVersion::merge(const GrpVersion & _grp)
{
    for(const auto &  vers1 : _grp)
    {
        bool found = false;
        for(const auto &  vers2 : *this)
        {
            //si base de nom identique
            if( vers1.cmpFile(vers2) )
            {
                found = true;
                break;
            }
        }

        if( !found )
            this->push_back(vers1);
        
    }
}


void GrpVersion::check(void)
{
    for( auto it = this->begin() ; it != this->end() - 1 ;it++)
    {
        for( auto it2 = it+1 ; it2 != this->end() ;it2++)
        {
            if( it->cmpBaseName(*it2) && !it->cmpFile(*it2) )
            {
                if( it->createDate != it2->createDate )
                {
                    it->add_error(Version::EF_CreateDate);
                    it2->add_error(Version::EF_CreateDate);
                }

                if( it->modifDate != it2->modifDate )
                {
                    it->add_error(Version::EF_ModifDate);
                    it2->add_error(Version::EF_ModifDate);
                }

                if( !utilitys::cmpFileSize( *this->atMainPath() +"\\"+it->get_subPathFile() , *this->atMainPath() +"\\"+it2->get_subPathFile()))
                {
                    it->add_error(Version::EF_MD5);
                    it2->add_error(Version::EF_MD5);
                }
                else
                {
                    if( !utilitys::cmpFilesHash( *this->atMainPath() +"\\"+it->get_subPathFile() , *this->atMainPath() +"\\"+it2->get_subPathFile()) )
                    {
                        it->add_error(Version::EF_MD5);
                        it2->add_error(Version::EF_MD5);
                    }
                    
                }
            }
        }
    }
}


class Device ;
typedef std::shared_ptr<std::vector<Device>> ptrVecDevice;
class Container ;
class TemplateGui : public MainPathSharedTemplate
{
   public:
      TemplateGui(void);

      virtual void addCssProvider(Glib::RefPtr<Gtk::CssProvider> _cssProvider);
      virtual void addParent(Gtk::Window* _parent);
      virtual void addDevice(ptrVecDevice _devices);

      Glib::RefPtr<Gtk::CssProvider> const getCssProvider(void);
      Gtk::Window* const getParent(void);
      ptrVecDevice const getDevice(void);

      Glib::RefPtr<Gtk::CssProvider> const atCssProvider(void);
      Gtk::Window* const atParent(void);
      ptrVecDevice const atDevice(void);


      virtual ~TemplateGui(void);
      
      private :
protected :
      Gtk::Window* m_parent;

      Glib::RefPtr<Gtk::CssProvider> m_cssProvider;

      ptrVecDevice m_devices;

};

TemplateGui::TemplateGui(void):MainPathSharedTemplate()
{
    this->m_parent = nullptr;
}

TemplateGui::~TemplateGui(void)
{
    this->m_parent = nullptr;
}

//setter
void TemplateGui::addCssProvider(Glib::RefPtr<Gtk::CssProvider> _cssProvider)
{
    this->m_cssProvider = _cssProvider;
}

//setter
void TemplateGui::addDevice( ptrVecDevice _devices )
{
    this->m_devices = _devices;
}

//setter
void TemplateGui::addParent(Gtk::Window* _parent)
{
    this->m_parent  = _parent;
}


//getter
Glib::RefPtr<Gtk::CssProvider> const TemplateGui::getCssProvider(void)
{
    return this->m_cssProvider;
}

//getter
Gtk::Window* const TemplateGui::getParent(void)
{
    return this->m_parent;
}

ptrVecDevice const TemplateGui::getDevice(void)
{
    return this->m_devices;
}

//acces securisé 
ptrVecDevice const TemplateGui::atDevice(void)
{
    if(!this->m_devices)
        throw std::logic_error("bad devices acces");

    return this->m_devices;
}
//acces securisé 
Glib::RefPtr<Gtk::CssProvider> const TemplateGui::atCssProvider(void)
{
    if(!this->m_cssProvider)
        throw std::logic_error("bad provider acces");

    return this->m_cssProvider;
}

//acces securisé 
Gtk::Window* const TemplateGui::atParent(void)
{
    if(this->m_parent == nullptr)
       throw std::logic_error("bad raw window acces");

    return this->m_parent;
}

class SwitchUser : public Gtk::HBox
{
   public :
      SwitchUser( bool _default = false , std::string const & _label ="");


      void set_label(std::string const & _label);
      bool get_active(void);
      void set_active(bool _stat);

     
       Glib::SignalProxyProperty  signal_changed(void);

   private:
      Gtk::Switch m_switchButton;
      Gtk::Label m_label;

} ;

SwitchUser::SwitchUser(bool _default  , std::string const & _label )
{
    this->pack_start( this->m_label , Gtk::PACK_SHRINK);
    this->pack_start( this->m_switchButton , Gtk::PACK_SHRINK );

    this->set_label(_label);
    this->set_active(_default);
}



void SwitchUser::set_label(std::string const & _label)
{
    this->m_label.set_label(_label + " ");
}

bool SwitchUser::get_active(void)
{
    return this->m_switchButton.get_active();
}

void SwitchUser::set_active(bool _stat)
{
    this->m_switchButton.set_active(_stat); 
    this->m_switchButton.property_active().signal_changed();
}

Glib::SignalProxyProperty  SwitchUser::signal_changed(void)
{
    return  this->m_switchButton.property_active().signal_changed();
}

class DialogThread : public Gtk::Dialog
{
   public :
      DialogThread(void);
      ~DialogThread(void);

      void run(void);
      void stop(void);

   private :

      bool thread_func(void);

       std::thread m_th;
       std::atomic<bool> m_continue , m_end ;
       Glib::Dispatcher m_signalDone;
      
};
DialogThread::DialogThread(void):Gtk::Dialog()
{
    this->m_signalDone.connect(sigc::mem_fun(this, &DialogThread::show_all));
    this->m_continue = false;
    this->m_end = false;
}

DialogThread::~DialogThread(void)
{
    this->stop();
}

void DialogThread::run(void)
{
    
    if(this->m_continue || this->m_th.joinable())
        return ;

    this->m_end = false;
    this->m_continue = true;
    this->m_th =  std::thread(&DialogThread::thread_func, this);

    
}

void DialogThread::stop(void)
{
    if(!this->m_continue)
        return;
    
    this->m_continue = false;
    if(this->m_th.joinable())
        this->m_th.join();

}

bool DialogThread::thread_func(void)
{
    while( this->m_continue )
    {
        this->m_signalDone.emit();

        const std::chrono::duration<double, std::milli> duration(500);

        std::this_thread::sleep_for(duration);
    }

    this->m_end = true;
    return true;
}


class VersionShower :  public TemplateGui , public Gtk::VBox , public std::vector<GrpVersion>
{
    public:

        VersionShower(void);
        void update_view( void);

    private :

       void on_clic(GrpVersion _cpy);
       void update_complet_path(void);
       void clipBoardCpy(void);

       Gtk::VBox id  , createDate , m_grpInfoPath , m_grpInfoError;
       GrpVersion m_current_sel;
       Gtk::Button m_groupeInfo , m_cpyPath;
       Gtk::Label m_createDate , m_modifDate , m_autor;

       Gtk::ComboBoxText m_version_sel, m_ext_sel ,m_part_sel;
       Gtk::Entry m_complet_path;

       DialogThread m_dialogInfoGrp;

       bool m_onclickLock;
};

VersionShower::VersionShower(void):TemplateGui(),Gtk::VBox()
{
    this->m_onclickLock = false;
    const int colSize = 2;
    Gtk::Frame *tmp[colSize] ;
    Gtk::Table *tab2 = Gtk::manage(new Gtk::Table( 2  , colSize ));  
    
    tab2->attach(*Gtk::manage(new Gtk::Label("id")), 0, 1, 0, 1 ,Gtk::SHRINK , Gtk::SHRINK); 
    tab2->attach(*Gtk::manage(new Gtk::Label("Date de création")), 1,2, 0, 1 , Gtk::SHRINK, Gtk::SHRINK);

    for(auto i =0 ; i < colSize ; i++)
    {
        tmp[i] =  Gtk::manage(new Gtk::Frame());
        tab2->attach(*tmp[i], i, i+1, 1, 2 ); 
    }

    tmp[0]->add( this->id );
    tmp[1]->add( this->createDate);
    
        
    Gtk::ScrolledWindow *sw =Gtk::manage(new Gtk::ScrolledWindow());
    sw->add(*tab2);

    this->pack_start(*sw);


    // parite gestion d'une selection
    Gtk::Table *tab3 = Gtk::manage(new Gtk::Table( 2  , 6 ));
    tab3->attach( *Gtk::manage(new Gtk::Label("")) , 0, 1, 0, 1 , Gtk::SHRINK , Gtk::SHRINK);
    tab3->attach( *Gtk::manage(new Gtk::Label("CompletePath")), 1, 2, 0, 1 ); 
    tab3->attach( *Gtk::manage(new Gtk::Label("P")), 2, 3, 0, 1 , Gtk::SHRINK , Gtk::SHRINK); 
    tab3->attach( *Gtk::manage(new Gtk::Label("V")) , 3, 4, 0, 1 , Gtk::SHRINK , Gtk::SHRINK);
    tab3->attach( *Gtk::manage(new Gtk::Label("E")), 4, 5, 0, 1 , Gtk::SHRINK , Gtk::SHRINK);
    tab3->attach( *Gtk::manage(new Gtk::Label("")), 5, 6, 0, 1 , Gtk::SHRINK , Gtk::SHRINK);

    tab3->attach( this->m_groupeInfo , 0, 1, 1, 2 , Gtk::SHRINK , Gtk::SHRINK);
    tab3->attach( this->m_complet_path, 1, 2, 1, 2 ); 
    tab3->attach( this->m_part_sel, 2, 3, 1, 2 , Gtk::SHRINK , Gtk::SHRINK); 
    tab3->attach( this->m_version_sel , 3, 4, 1, 2 , Gtk::SHRINK , Gtk::SHRINK);
    tab3->attach( this->m_ext_sel, 4, 5, 1, 2 , Gtk::SHRINK , Gtk::SHRINK);
    tab3->attach( this->m_cpyPath, 5, 6, 1, 2 , Gtk::SHRINK , Gtk::SHRINK);

    this->m_complet_path.set_editable(false);

    this->m_groupeInfo.signal_clicked().connect(  sigc::mem_fun(this->m_dialogInfoGrp ,&DialogThread::show_all)  );
    this->m_version_sel.signal_changed().connect(  sigc::mem_fun(*this,&VersionShower::update_complet_path)  );
    this->m_ext_sel.signal_changed().connect(  sigc::mem_fun(*this,&VersionShower::update_complet_path)  );
    this->m_cpyPath.signal_clicked().connect(  sigc::mem_fun(*this,&VersionShower::clipBoardCpy)  );

    

    Gtk::Table *infoBare = Gtk::manage(new Gtk::Table( 2  , 3 ));

    infoBare->attach( *Gtk::manage(new Gtk::Label("creatDate")) , 0, 1, 0, 1 );
    infoBare->attach(  *Gtk::manage(new Gtk::Label("modif date")), 1, 2, 0, 1 ); 
    infoBare->attach(  *Gtk::manage(new Gtk::Label("autor")), 2, 3, 0, 1 ); 

    infoBare->attach(this->m_createDate , 0, 1, 1, 2 );
    infoBare->attach( this->m_modifDate, 1, 2, 1, 2 ); 
    infoBare->attach( this->m_autor, 2, 3, 1, 2 ); 

    this->pack_end(*infoBare , Gtk::PACK_SHRINK);
    
    this->pack_end(*tab3 , Gtk::PACK_SHRINK);


    this->m_dialogInfoGrp.signal_hide().connect(sigc::mem_fun( this->m_dialogInfoGrp, &DialogThread::stop));
    this->m_dialogInfoGrp.signal_show().connect(sigc::mem_fun( this->m_dialogInfoGrp, &DialogThread::run));
    this->m_dialogInfoGrp.set_title( "Grp Info");
    this->m_dialogInfoGrp.set_resizable(true  );

     this->m_dialogInfoGrp.set_default_size(700, 480);
     this->m_dialogInfoGrp.maximize();
    
    this->m_dialogInfoGrp.set_position(Gtk::WIN_POS_CENTER);
    Gtk::Table *infoGrp = Gtk::manage(new Gtk::Table( 2  , 2 ));

    infoGrp->attach( *Gtk::manage(new Gtk::Label("Path File")) , 0, 1, 0, 1 , Gtk::SHRINK , Gtk::SHRINK);
    infoGrp->attach( *Gtk::manage(new Gtk::Label("Error")) , 1, 2, 0, 1 , Gtk::SHRINK , Gtk::SHRINK);

    infoGrp->attach( this->m_grpInfoPath , 0, 1, 1, 2 ,Gtk::SHRINK , Gtk::SHRINK);
    infoGrp->attach( this->m_grpInfoError ,1, 2, 1, 2 ,Gtk::SHRINK , Gtk::SHRINK);

    Gtk::ScrolledWindow *sw2 =Gtk::manage(new Gtk::ScrolledWindow());
    sw2->set_policy(Gtk::PolicyType::POLICY_AUTOMATIC, Gtk::PolicyType::POLICY_AUTOMATIC);
    sw2->add(*infoGrp);
    sw2->set_vexpand(true);
    this->m_dialogInfoGrp.get_content_area()->add( *sw2 );
}
void VersionShower::clipBoardCpy(void)
{
    if(this->m_current_sel.size() == 0)
        return ;

    utilitys::cpyToPP(  this->m_complet_path.get_text() );

}
void VersionShower::update_view( void)
{

    utilitys::gtkmmRemoveChilds(this->id);
    utilitys::gtkmmRemoveChilds(this->createDate);

    for(GrpVersion & grp : *this)
    {
        auto bp = Gtk::manage(new Gtk::Button( grp.get_id().size() > 0 ? grp.get_id() : grp.get_name() , Gtk::PACK_SHRINK ));

        bp->signal_clicked().connect(sigc::bind<GrpVersion>(sigc::mem_fun(*this,&VersionShower::on_clic), grp));
        this->id.pack_start(*bp,Gtk::PACK_SHRINK);


        auto lab2 = Gtk::manage(new Gtk::Button(grp.get_createDate() , Gtk::PACK_SHRINK ));

        this->createDate.pack_start(*lab2,Gtk::PACK_SHRINK);
    }
    


    this->show_all();
}

void VersionShower::on_clic(GrpVersion _cpy)
{
    //deja un signal en cour
    if(this->m_onclickLock)
        return ;

    this->m_onclickLock = true;
    this->m_current_sel = _cpy;

    
    //on ajoute les extension existante 
    this->m_ext_sel.remove_all();

    std::vector<std::string> tmp;

    for( const auto & vers : _cpy)
    {
        if(  vers.extension.size() > 0 ) 
        {   
            bool found = false;

            for(const auto & str : tmp)
            {
                if(str == vers.extension  )
                {
                    found = true ;
                    break;
                }
            }

            if(!found && !vers.get_error())
                tmp.push_back( vers.extension );
        }
    }

     for(const auto & str : tmp)
     {
        this->m_ext_sel.append(str);
     }

    
    if(tmp.size() > 0)
    {
         this->m_ext_sel.set_active_text(tmp.front());
         tmp.clear();
    }
   

    
    //ajoute les version existante
    this->m_version_sel.remove_all();
    for( const auto & vers : _cpy)
    {
        if(  vers.version.size() > 0 ) 
        {
            bool found = false;
            for(const auto & str : tmp)
            {
                if(str == vers.version)
                {
                    found = true ;
                    break;
                }
            }

            if(!found && !vers.get_error())
                tmp.push_back( vers.version );
        }
    }

     for(const auto & str : tmp)
        this->m_version_sel.append(str);
     
     
    if(tmp.size() > 0)
    {
        this->m_version_sel.set_active_text( tmp.back() );
        tmp.clear();
    }
    
    
    //ajoute les partie existante
    this->m_part_sel.remove_all();
    for( const auto & vers : _cpy)
    {
        if(  vers.part.size() > 0 ) 
        {
            bool found = false;
            for(const auto & str : tmp)
            {
                if(str == vers.part)
                {
                    found = true ;
                    break;
                }
            }

            if(!found && !vers.get_error())
                tmp.push_back( vers.part );
        }
    }

     for(const auto & str : tmp)
        this->m_part_sel.append(str);
     
     

    if(tmp.size() > 0)
    {
        this->m_part_sel.set_active_text( tmp.back() );
        tmp.clear();
    }
    

    //completel'infobare
    

    this->m_autor.set_label( _cpy.get_autor() );
    this->m_createDate.set_label( _cpy.get_createDate() );
    this->m_modifDate.set_label( _cpy.get_createDate() );


    this->update_complet_path();

    utilitys::gtkmmRemoveChilds(this->m_grpInfoPath);
    utilitys::gtkmmRemoveChilds(this->m_grpInfoError);

    for( const auto & vers : this->m_current_sel)
    {
        this->m_grpInfoPath.pack_start(*Gtk::manage( new Gtk::Button(*this->atMainPath()+"\\"+ vers.dir + "\\" + vers.name +"." +vers.extension)),Gtk::PACK_SHRINK);

        this->m_grpInfoError.pack_start(*Gtk::manage( new Gtk::Button( Version::VersionError(vers) )),Gtk::PACK_SHRINK);
    }


    this->m_onclickLock = false;

}

void VersionShower::update_complet_path(void)
{
    for(const auto & vers : this->m_current_sel)
    {
        if( this->m_ext_sel.get_active_text() == vers.extension  && this->m_part_sel.get_active_text() == vers.part && this->m_version_sel.get_active_text() == vers.version)
        {
            this->m_complet_path.set_text(*this->atMainPath()+"\\"+ vers.dir + "\\" + vers.name +"." +vers.extension);
            break;
        }
    }
}

//class qui définit un dossier comme conteneur
class Container : public std::vector<GrpVersion> 
{
    public: 
        Container( const std::string & _name);
        const std::string get_name(void)const;
        void addVersionShower(std::shared_ptr<VersionShower> _ptr_vs);
        const std::shared_ptr<VersionShower> & atVersionShower(void);

        void set_authExt(const std::string & _authExt);
        void set_reg_version(const std::string & _str);
        void set_reg_id(const std::string & _str);
        void set_reg_autor(const std::string & _str);
        void set_reg_part(const std::string & _str);

        const std::string get_authExt(void) const;

        const std::string get_reg_version(void) const;
        const std::string get_reg_id(void) const;
        const std::string get_reg_autor(void) const;
        const std::string get_reg_part(void) const;


        void add2GrpVersion(const Version & _v);
    protected:

    const std::string m_name;
    std::shared_ptr<VersionShower> m_ptr_vs;
    std::string m_authext;

    std::string m_reg_version  , m_reg_autor , m_reg_id , m_reg_part;
};

Container::Container( const std::string & _name): std::vector<GrpVersion>() ,  m_name(_name)
{
    this->m_authext = "";
}
const std::string Container::get_name(void)const
{
    return this->m_name;
}

void Container::addVersionShower(std::shared_ptr<VersionShower> _ptr_vs)
{
    this->m_ptr_vs = _ptr_vs;
}
const std::shared_ptr<VersionShower> & Container::atVersionShower(void)
{
    if(!this->m_ptr_vs)
        throw std::logic_error("bad VersionShower acces");

    return this->m_ptr_vs;
}

void  Container::add2GrpVersion(const Version & _v)
{
    for(auto & grp : *this)
    {
        if(grp.get_name() == _v.name)
        {
            grp.push_back(_v);
            return ;
        }
    }

    GrpVersion tmp;
    tmp.push_back(_v);

    this->push_back(tmp);
}

void Container::set_authExt(const std::string & _authExt)
{
    this->m_authext = _authExt;
}
const std::string Container::get_authExt(void) const
{
    return this->m_authext;
}

void Container::set_reg_version(const std::string & _str)
{
    this->m_reg_version = _str;
}
void Container::set_reg_id(const std::string & _str)
{
    this->m_reg_id = _str;
}
void Container::set_reg_autor(const std::string & _str)
{
    this->m_reg_autor = _str;
}
void Container::set_reg_part(const std::string & _str)
{
    this->m_reg_part = _str;
}


const std::string Container::get_reg_version(void) const
{
    return this->m_reg_version;
}
const std::string Container::get_reg_id(void) const
{
    return this->m_reg_id;
}
const std::string Container::get_reg_autor(void) const
{
    return this->m_reg_autor;
}
const std::string Container::get_reg_part(void) const
{
    return this->m_reg_part;
}



//class qui definit un dossier comme materiel
class Device 
{
    public:
        Device( const std::string & _name);
        std::vector<Container> & get_containers(void);
        const std::string get_name(void)const;
        void update( const std::string & _mainPath , const std::vector<std::string> & _files,bool _forceUpdate = false);
    protected:

        std::vector<Container> m_containers;
        const std::string m_name;

        bool m_update;

};

Device::Device( const std::string & _name):m_name(_name),m_update(false)
{
}

std::vector<Container> & Device::get_containers(void)
{
    return this->m_containers;
}

const std::string Device::get_name(void)const
{
    return this->m_name;
}

void Device::update( const std::string & _mainPath , const std::vector<std::string> & _files , bool _forceUpdate)
{

    if( this->m_update && !_forceUpdate)
        return ;

        //part du principe qu'on est pas dans une racite materiel
    std::vector<std::string>  pathDevice ;

    for(auto & f : _files)
    {
        auto tmp = utilitys::sep_sub_and_name( f );
        
        bool found =false;
        //! ici on cherche u materiel
        
        while ( std::get<0>(tmp).size() > 0)
        {
            tmp = utilitys::sep_sub_and_name( std::get<0>(tmp) );

            std::string tmpName = std::get<1>(tmp);
            utilitys::upper(tmpName);
            
            if( tmpName == this->m_name )
            {
                found = true;
                break;
            }
        }
        
        if( found )
        {
                pathDevice.push_back( f.substr(_mainPath.size() , f.size()) );
                
                //supprime la racine si presente
                if(pathDevice.back().front() == '/' || pathDevice.back().front() == '\\')
                    pathDevice.back().erase(pathDevice.back().begin());
        }
           
    }


    //recherche les differant contenaire
    
    for(auto & c : m_containers)
    {
        c.clear();

        for(const auto &  pd : pathDevice)
        {
            std::string tmpStr = pd;

            utilitys::upper(tmpStr);
            auto found = tmpStr.find( c.get_name() );

            if( found!=std::string::npos)
            {
                
                auto pathName = utilitys::sep_sub_and_name(pd);
                auto nameExt = utilitys::sep_name_and_ext(std::get<1>(pathName));
                
                //ajoute si l'extension est supporté
                if( c.get_authExt().find( std::get<1>(nameExt) ) != std::string::npos )
                {
                    Version tmp ;
                    tmp.dir = std::get<0>(pathName);
                    
                    tmp.name = std::get<0>(nameExt);
                    tmp.id = utilitys::regSearch(c.get_reg_id() , tmp.name);
                    
                    if(tmp.id.size() == 0)
                        tmp.id = tmp.name;

                    tmp.autor =  utilitys::regSearch(c.get_reg_autor() , tmp.name);
                    tmp.part =  utilitys::regSearch(c.get_reg_part() , tmp.name);
                    tmp.version =  utilitys::regSearch(c.get_reg_version() , tmp.name);

                    
                    tmp.extension = std::get<1>(nameExt);

                    std::string tmpFileName =  _mainPath + "\\"+ tmp.dir +"\\"+ tmp.name + "."+ tmp.extension;


                    // Obtenez le temps de dernière modification du fichier
                    std::filesystem::file_time_type lastWriteTime = std::filesystem::last_write_time(tmpFileName);

                    tmp.modifDate = std::string(std::format("{}",lastWriteTime)).substr(0,10) ;
                    tmp.createDate = utilitys::get_createDate(tmpFileName);
                    
                    // Convertissez std::time_t en une chaîne de caractères représentant la date et l'heure
                    

                    c.add2GrpVersion(tmp);
                }
                
            }
        } 

    }
    pathDevice.clear();
    pathDevice.shrink_to_fit();
    
    this->m_update  =true;
}

static std::tuple<std::string,ptrVecDevice> load(const std::string & _ressourcePath)
{
    std::clog << "load configuration ...   " << std::endl;
    const auto start{std::chrono::steady_clock::now()};

    ptrVecDevice devices;


    //charge les ficheir de maniere asynchrone // utlise 3 thread
    std::future< std::vector<std::string> > deviceTh = std::async ( utilitys::readList, _ressourcePath + "\\devicesList.csv" , true);
    std::future< std::vector<std::string> > contTh = std::async ( utilitys::readList, _ressourcePath + "\\containerList.csv" , true);
    std::future< std::vector<std::string> > parameterTh = std::async (utilitys::readList, _ressourcePath + "\\parameters.csv" , false);
    
    std::vector<std::string>&& lsContainer = contTh.get();
    std::vector<std::string>&& lsParameter = parameterTh.get();
    std::vector<std::string>&& lsDevices = deviceTh.get();


    if(  lsContainer.size() == 0)
        throw std::logic_error("no containers found");
    
    if( lsDevices.size() == 0 )
        throw std::logic_error("no devices subpath found");
    
    if(lsParameter.size() == 0)
        throw std::logic_error("no parametres found");


    std::string && mainPath =  utilitys::getParamValue("MAIN_PATH" , lsParameter , true);


    if(mainPath.size() == 0)
        throw std::logic_error("no mainPath  found");

    if(mainPath.back() == '\\' || mainPath.back() == '/')
        mainPath.pop_back();


    devices = ptrVecDevice(new std::vector<Device>());

    //contruis une liste de contenaire patron pour ne pas repeter getParamValue
    std::vector<Container> tmpCont;
    for(const auto & cnt : lsContainer)
    {
        tmpCont.emplace_back( cnt );
        tmpCont.back().set_authExt( std::move(utilitys::getParamValue("AUTHEXT[" + cnt +"]" , lsParameter , true)) );

        tmpCont.back().set_reg_version( std::move(utilitys::getParamValue("VERSIONREGEX[" + cnt +"]" , lsParameter , true)));
        tmpCont.back().set_reg_id( std::move(utilitys::getParamValue("IDREGEX[" + cnt +"]" , lsParameter , true)));
        tmpCont.back().set_reg_autor( std::move(utilitys::getParamValue("AUTORREGEX[" + cnt +"]" , lsParameter , true)));
        tmpCont.back().set_reg_part( std::move(utilitys::getParamValue("PARTREGEX[" + cnt +"]" , lsParameter , true)));
    }

    //construit la liste de materiel
    for(const auto & name : lsDevices)
    {
        //cherche si pas deja inlcu

        bool found = false;
        for( const auto & dev : *devices)
            if(name == dev.get_name())
                found = true;

        //si le cas passe au suivant
        if( found)
            continue;

        //ajoute un materiel
        devices->emplace_back( name );

        //ajoute les contenaire au materiel
        for(const auto & cnt : tmpCont)
        {
            devices->back().get_containers().emplace_back( std::move(cnt.get_name()) );
            devices->back().get_containers().back().set_authExt( std::move(cnt.get_authExt()));

            devices->back().get_containers().back().set_reg_version(  std::move(cnt.get_reg_version()) );
            devices->back().get_containers().back().set_reg_id(  std::move(cnt.get_reg_id()) );
            devices->back().get_containers().back().set_reg_autor(  std::move(cnt.get_reg_autor()) );
            devices->back().get_containers().back().set_reg_part(  std::move(cnt.get_reg_part()) );
        }
    }

    const auto end{std::chrono::steady_clock::now()};
    const std::chrono::duration<double> elapsed_seconds{end - start};

    std::clog <<"succes load in : " << elapsed_seconds.count() * 1000 << " ms" <<std::endl;

    return {mainPath , devices};
}



class AutoCompletVisualiz :  public TemplateGui , public Gtk::ScrolledWindow
{
    public:
         AutoCompletVisualiz(void);

         void update(std::vector<std::string> const & list);

         sigc::signal<void ,std::string> & signal_select(void);

    private:
    Gtk::HBox m_hbox;
    std::vector<Gtk::Button> m_button;

    void signal_clicked(std::string const & value);


    sigc::signal<void , std::string > m_signalSelect;

};
AutoCompletVisualiz::AutoCompletVisualiz(void):TemplateGui(), Gtk::ScrolledWindow()
{
    this->add( this->m_hbox );
}

void AutoCompletVisualiz::signal_clicked(std::string const & value)
{
    this->m_signalSelect.emit(value);
}

 sigc::signal<void ,std::string> & AutoCompletVisualiz::signal_select(void)
 {
    return this->m_signalSelect;
 }

void AutoCompletVisualiz::update(std::vector<std::string> const & list)
{
    for(auto it = this->m_button.begin() ; it != this->m_button.end() ; it++)
    {
        
        this->m_hbox.remove(*it);
    }

    this->m_button.clear();

    for(size_t i =0 ; i <  list.size() ;i++)
    {
        this->m_button.push_back( Gtk::Button(list[i]));
        this->m_hbox.pack_start( this->m_button.back() ,  Gtk::PACK_SHRINK);

        this->m_button.back().set_can_focus(false);
        this->m_button.back().set_focus_on_click(false);
        this->m_button.back().signal_clicked().connect( sigc::bind<std::string>(sigc::mem_fun(*this, &AutoCompletVisualiz::signal_clicked),list[i]));
    }

    this->m_hbox.show_all();
}

//

class Prompt  :  public TemplateGui , public Gtk::VBox
{
    public : 

        Prompt(void);

    private:
        void completionList(void);
        void completion(std::string const & value);
        void enterSignal(void);

        void helpSignal(void);

        int findDualChar(std::string const & line , const char c1 , const char c2);


        Gtk::Entry m_entry;
        AutoCompletVisualiz m_autoCompletion;
        SwitchUser m_swu_update;
        Gtk::Button m_help;

};

Prompt::Prompt(void):TemplateGui() ,  Gtk::VBox()
{
    auto hbox = Gtk::manage( new Gtk::HBox() );

    this->m_entry.set_can_focus(true);
    this->m_entry.set_focus_on_click(true);
    this->m_entry.grab_focus();
    this->m_entry.set_alignment(Gtk::ALIGN_START);
    this->m_entry.set_placeholder_text("Entrer un materiel");

    this->m_entry.set_max_length(200);
    this->m_entry.signal_activate().connect(sigc::mem_fun(*this, &Prompt::enterSignal));
    this->m_entry.signal_changed().connect( sigc::mem_fun( *this , &Prompt::completionList ));

    this->m_autoCompletion.signal_select().connect( sigc::mem_fun( *this , &Prompt::completion ));

    this->m_help.signal_clicked().connect( sigc::mem_fun( *this , &Prompt::helpSignal));

    this->m_swu_update.set_label("update");
    this->m_swu_update.set_active(false);
    this->m_swu_update.signal_changed().connect(sigc::mem_fun(*this, &Prompt::enterSignal));

    
    hbox->pack_start(this->m_entry);
    hbox->pack_start(this->m_swu_update , Gtk::PACK_SHRINK);
    hbox->pack_start(this->m_help, Gtk::PACK_SHRINK);

    this->pack_start(this->m_autoCompletion);
    this->pack_start(*hbox);
}

void Prompt::helpSignal(void)
{
    utilitys::openWebPage("https://github.com/jeromefavrou/MFVGT/wiki");
}

void Prompt::enterSignal(void)
{
    const auto start{std::chrono::steady_clock::now()};

    std::vector< std::string > lsAtSearch , lsAtIgnor;
    
    std::string && current_entry = this->m_entry.get_text();

    if(current_entry.size() ==0)
        return;

    auto res = utilitys::sep_string<';'>( current_entry );
    lsAtSearch.push_back( std::move(std::get<0>(res)) );

    while( std::get<1>(res).size() > 0 )
    {
        res = utilitys::sep_string<';'>( std::get<1>(res) );

        if( std::get<0>(res).front() == '!' )
        {
            lsAtIgnor.push_back( std::get<0>(res).substr(1 , std::get<0>(res).size() )) ;
        }
        else
        {
            lsAtSearch.push_back( std::move(std::get<0>(res)) );
        }
        
    }

    //cherche si l'entrée est valide


    std::vector<std::string> files ;

    // Utiliser la fonction récursive pour lister les fichiers
    utilitys::listerFichiers(*this->atMainPath(), files);


    //update
    for(auto const & lsDev : lsAtSearch)
    {
        std::string current = lsDev;

        bool recurs = current.back() == '*' ? true : false;

        if(recurs)
            current.erase(current.end()-1);

        for(auto & dev : *this->atDevice() )
        {
            bool maybeIgnor = false;

            for(auto const & lsI : lsAtIgnor )
            {
                bool recursBan = lsI.back() == '*' ? true : false;

                if( (dev.get_name() == lsI  && !recursBan) || (dev.get_name().find(lsI.substr(0 , lsI.size() -1 ) ) != std::string::npos && recursBan) )
                {
                    maybeIgnor = true;

                    break;
                }
            }

            if(maybeIgnor)
                continue;

            if( std::find(lsAtIgnor.begin(), lsAtIgnor.end(), dev.get_name()) != lsAtIgnor.end() )
            {
                continue ;
            }
            if( (dev.get_name() ==  current && !recurs ) || (dev.get_name().find(current) != std::string::npos && recurs))
            {
                dev.update( *this->atMainPath() , files , this->m_swu_update.get_active() );

                if(!recurs)
                    break;
            }
        }
    }
    

    //nettoyage des vue
    for( auto & cont : this->atDevice()->front().get_containers() )
        cont.atVersionShower()->clear();
    
    //ajoute les groupe a afficher

    for(auto const & lsDev : lsAtSearch)
    {
        std::string current = lsDev;

        bool recurs = current.back() == '*' ? true : false;

        if(recurs)
            current.erase(current.end()-1);

        for(auto & dev : *this->atDevice() )
        {

            bool maybeIgnor = false;

            for(auto const & lsI : lsAtIgnor )
            {
                bool recursBan = lsI.back() == '*' ? true : false;

                if( (dev.get_name() == lsI  && !recursBan) || (dev.get_name().find(lsI.substr(0 , lsI.size() -1 ) ) != std::string::npos && recursBan) )
                {
                    maybeIgnor = true;

                    break;
                }
            }

            if(maybeIgnor)
                continue;

            if( (dev.get_name() ==  current  && !recurs ) || ( dev.get_name().find(current) != std::string::npos && recurs))
                for( auto & cont : dev.get_containers() )
                    for(auto & grp : cont)
                        cont.atVersionShower()->push_back(grp);
        }
            

    }

    //mise a jour des vue
    for( auto & cont : this->atDevice()->front().get_containers() )
    {
        // fusionne les groupeVersion
        for( auto it = cont.atVersionShower()->begin(); it != cont.atVersionShower()->end() ; it++ )
        {
            if(it +1 <  cont.atVersionShower()->end())
            {
                for( auto it2 = it+1 ; it2 != cont.atVersionShower()->end() ; it2++ )
                {
                    if( it->get_id() == it2->get_id() )
                    {
                        it->merge(*it2);
                        it=cont.atVersionShower()->begin()-1;
                        cont.atVersionShower()->erase(it2);
                        break;
                    }
                } 
            }
        }
        
        //verifie si il a des incoherence entre fichier
        for(auto & grpShow : *cont.atVersionShower())
        {
            grpShow.addMainPath( this->getMainPath() );
            grpShow.check();
        }

        //mise a jour des vue
        cont.atVersionShower()->update_view();

    }
        


    const auto end{std::chrono::steady_clock::now()};
    const std::chrono::duration<double> elapsed_seconds{end - start};

    std::clog <<"succes load entry in : " << elapsed_seconds.count() * 1000 << " ms" <<std::endl;
    

}

void Prompt::completion(std::string const & value)
{
    std::string _prompt = this->m_entry.get_text();
    std::string last;

     //garde que le dernier segment ( separer par espace )
    for(auto i = _prompt.size() ;i >0 ; i--)
    {
        if( _prompt[i] == ';')
        {
            last = std::string(_prompt.begin() ,_prompt.begin() + i +1);
            _prompt.erase(_prompt.begin() ,_prompt.begin() + i +1 );
            break;
        }
    }

    bool dot = false;
    unsigned int posLastDot = 0 ;

    for( size_t i = 0 ; i < _prompt.size() ; i++)
    {
        if(_prompt[i] == '.')
        {
            dot = true;
            posLastDot = i;
        }
    }

    if(!dot)
        _prompt = "";
    else
    {
        _prompt.erase(_prompt.begin() + posLastDot ,_prompt.end() );
        _prompt += ".";
    }

    this->m_autoCompletion.update({});
    this->m_entry.set_text(last + _prompt + value);
    this->m_entry.set_position(this->m_entry.get_text_length());
    this->m_entry.activate();
}

void Prompt::completionList(void)
{
    std::string currentText = this->m_entry.get_text(); 

    if(currentText.size() == 0)
        return ;

    if(currentText.front() == '!')
    {
        currentText.erase( currentText.begin()); 
    }

     
    //permission de caractere
    for(std::string::iterator it = currentText.begin() ; it != currentText.end() ; it++ )
    {
        bool dec =  *it >= 0x30 && *it <= 0x39;
        bool maj =  *it >= 0x41 && *it <= 0x5A;
         bool min =  *it >= 0x61 && *it <= 0x7A;
         bool star = *it == '*' || *it == ';' || *it == '!';
        if( !dec && !maj && !min && !star)
        {
            currentText.erase(it);
            it = currentText.begin() ;
        }

        //upper
        if(min)
        {
            *it = *it - char(32) ;
        }
    }



    this->m_entry.set_text(currentText);

    

    if( currentText.size() == 0 || currentText.back() =='*')
        return ;

    currentText = std::get<1>(  utilitys::sep_string<';'>(currentText, true) );

    std::vector<std::string> autoComplet;

    for(auto & device : *this->m_devices)
    {
        std::string tmp  =device.get_name();
        std::size_t found = tmp.find( currentText );
        // si trouver a l'index 0
        if (found!=std::string::npos && found == 0)
        {
            //verifie que pas deja present
             auto it = std::find( autoComplet.begin(),autoComplet.end() ,tmp );

             if(it == autoComplet.end())
                autoComplet.push_back(tmp);
        }
    }

    this->m_autoCompletion.update(autoComplet);

}






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

    this->m_utilitysPanel.pack2( *Gtk::manage(new Gtk::Button( "PDF" )) );

    this->m_usersPanel.pack_start(this->m_prompt  , Gtk::PACK_SHRINK);

    if( this->atDevice()->size() > 0 )
    {
        for(auto & cont : this->atDevice()->front().get_containers() )
        {
            cont.addVersionShower( std::shared_ptr< VersionShower>( new VersionShower() ) ) ;
            cont.atVersionShower()->addMainPath( this->atMainPath());
            this->m_nbk.append_page( *cont.atVersionShower() ,  *Gtk::manage(new Gtk::Label( cont.get_name() )) );
        }

        //propage sur tout les device mes shower
        for(auto & dev : *this->atDevice() )
        {
            for(size_t i = 0 ; i < dev.get_containers().size() ; i++ )
            {
                dev.get_containers().at(i).addVersionShower( this->atDevice()->front().get_containers().at(i).atVersionShower() ) ;
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

